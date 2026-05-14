#include "Server.hpp"
#include "Parsing.hpp"
#include <errno.h>

volatile sig_atomic_t g_running = 1;

void signalHandler(int)
{
	g_running = 0;
}

Server::Server(const int &port, const std::string &password)
    : _port(port), _password(password)
{
	socketInit();
}

void Server::socketInit()
{
	_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketfd < 0)
        throw std::runtime_error("Socket creation failed");

	if (fcntl(_socketfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl failed to set O_NONBLOCK");
	
	//para poder reutilizar el puntero
	int n = 1;
        if (setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n)) < 0)
                throw std::runtime_error("setsockopt failed");

	sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_port);

	if (bind(_socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                throw std::runtime_error("Bind failed");
	if (listen(_socketfd, SOMAXCONN) < 0)
                throw std::runtime_error("Listen failed");
}

void Server::start()
{
	signal(SIGINT, signalHandler);
	// Añades el socket del servidor a la lista de poll
	pollfd server_fd;
	server_fd.fd = _socketfd;
	server_fd.events = POLLIN;
	server_fd.revents = 0;
	_poll_fds.push_back(server_fd);

	while (g_running) 
    {
		int ret = poll(_poll_fds.data(), _poll_fds.size(), -1); //_poll_fds.data() da acceso al array interno del vector y poll_fds.size() es cuántos sockets vamos a vigilar. El -1 es para que lo haga permanentemente.
        
		if (ret < 0)
		{
			if (errno == EINTR && !g_running)
				break;

			throw std::runtime_error("poll failed");
		}

                // Recorre todos los sockets registrados para ver cuáles tienen eventos pendientes
		for (size_t i = 0; i < _poll_fds.size(); ++i) 
        {
			//  Si el socket que tiene evento es el del servidor (_socketfd), significa que hay una nueva conexión entrante
			if (_poll_fds[i].fd == _socketfd && (_poll_fds[i].revents & POLLIN)) 
            {
				int client_fd = accept(_socketfd, NULL, NULL);
				if (client_fd < 0)
					continue;

				// modo no bloqueante
				fcntl(client_fd, F_SETFL, O_NONBLOCK);

				// crear el objeto Client para la nueva conexión
				Client* new_client = new Client("", "", client_fd);

				clients.push_back(new_client);

                 // se registra el nuevo cliente en poll_fds, para que poll() esté atento a lo que envía ese cliente.
				pollfd client_poll;
				
				client_poll.fd = client_fd;
				client_poll.events = POLLIN;
				client_poll.revents = 0;

				_poll_fds.push_back(client_poll);

				_clients_buffer[client_fd] = ""; // inicializar el buffer para ese cliente
				
				std::cout << "Welcome to IRC" << std::endl;
				std::cout << "New client connected: fd=" << client_fd << std::endl;
			}
			// Si no es el servidor, entonces es un cliente con datos listos para leer
			else if(_poll_fds[i].revents & POLLIN)
			{
				int fd = _poll_fds[i].fd;
				char buffer[512];
				ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
			
				if (bytes <= 0) 
				{
					close(fd);
					_clients_buffer.erase(fd);
					removeClientBySocket(fd);
					_poll_fds.erase(_poll_fds.begin() + i);
					--i;
					std::cout << "Client disconnected: fd=" << fd << std::endl;
					continue;
				}
				buffer[bytes] = '\0';
				_clients_buffer[fd] += buffer;
				std::string& client_buffer = _clients_buffer[fd];
				size_t pos;
				
				while ((pos = client_buffer.find("\r\n")) != std::string::npos) 
				{
					std::string msg = client_buffer.substr(0, pos);
					client_buffer.erase(0, pos + 2);
					if (msg.empty())
						continue;
				
					try
					{
						Client* client = getClientBySocket(_poll_fds[i].fd);
						Parsing p(msg, this, client);
						if (client && client->isDisconnected())
    					{
    					    close(fd);
    					    _clients_buffer.erase(fd);
    					    removeClientBySocket(fd);
    					    _poll_fds.erase(_poll_fds.begin() + i);
    					    --i;
    					    break; // salir del while de mensajes, el cliente ya no existe
    					}
						for (size_t j = 0; j < _poll_fds.size(); ++j)
						{
							if (_poll_fds[j].fd == _socketfd)
								continue;
							Client* pending = getClientBySocket(_poll_fds[j].fd);
							if (pending && pending->hasPendingMessages())
								_poll_fds[j].events |= POLLOUT;
						}
					}
					catch(Parsing::InvalidNickException&)
					{
						std::string err = "432: Invalid Nickname\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					catch(Parsing::NotRegisteredException&)
					{
						std::string err = "451: Client not registered\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					catch(Parsing::NeedMoreParamsException&)
					{
						std::string err = "461: not enough parameters\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					catch(Parsing::TooManyParamsException&)
					{
						std::string err = "461: too many parameters\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					catch(Parsing::UnknownCommandException&)
					{
						std::string err = "421: unknown command\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					catch(Parsing::WrongPasswordException&)
					{
						std::string err = "464: Password Incorrect\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					catch(Parsing::NicknameInUseException&)
					{
						std::string err = "433: Nick name in use\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					catch(Parsing::MayNotReRegisterException&)
					{
						std::string err = "462: May not reregister\r\n";
						send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
					}
					//std::cout << "[recv] " << msg << std::endl; 
				}
			}
			else if (_poll_fds[i].revents & POLLOUT) 
			{
				int fd = _poll_fds[i].fd;

				Client* client = getClientBySocket(fd);
				
				if (client == NULL)
					continue;
				
				client->flushSendBuffer();
				if (client->getSendBuffer().empty())
					_poll_fds[i].events &= ~POLLOUT;
			}
		}
	}
	std::cout << "\nClosing server..." << std::endl;

	for (size_t i = 0; i < clients.size(); ++i)
	{
		int fd = clients[i]->getClientSocket();

		if (fd >= 0)
			close(fd);
	}

	if (_socketfd >= 0)
		close(_socketfd);

	std::cout << "Server stopped cleanly"
			  << std::endl;
}

int Server::getSocketFD() const 
{
    return _socketfd;
}
Client* Server::getClientBySocket(int socket)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i]->getClientSocket() == socket)
			return clients[i];
	}
	return NULL;
}

void Server::removeClientBySocket(int socket)
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i]->getClientSocket() == socket)
		{
			delete clients[i];
			clients.erase(clients.begin() + i);
			return;
		}
	}
}
Server::~Server()
{
    for (size_t i = 0; i < channels.size(); ++i)
		delete channels[i];
	for (size_t i = 0; i < clients.size(); ++i)
		delete clients[i];
    close(_socketfd);
}

std::string	Server::get_password() const
{
	return (_password);
}

bool	Server::findClientByNick(std::string& nick)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i]->getNickname() == nick)
			return true;
	}
	return false;
}

bool	Server::findClientByUser(std::string user)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i]->getUsername() == user)
			return true;
	}
	return false;
}

