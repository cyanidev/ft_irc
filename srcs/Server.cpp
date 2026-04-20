#include "Server.hpp"
#include "Parsing.hpp"

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
	// Añades el socket del servidor a la lista de poll
	int client_fd;
	pollfd server_fd;
	server_fd.fd = _socketfd;
	server_fd.events = POLLIN;
	_poll_fds.push_back(server_fd);

	while (true) 
    {
		int ret = poll(_poll_fds.data(), _poll_fds.size(), -1); //_poll_fds.data() da acceso al array interno del vector y poll_fds.size() es cuántos sockets vamos a vigilar. El -1 es para que lo haga permanentemente.
        if (ret < 0)
			throw std::runtime_error("poll failed");

                // Recorre todos los sockets registrados para ver cuáles tienen eventos pendientes
		for (size_t i = 0; i < _poll_fds.size(); ++i) 
        {
			//  Si el socket que tiene evento es el del servidor (_socketfd), significa que hay una nueva conexión entrante
			if (_poll_fds[i].fd == _socketfd && (_poll_fds[i].revents & POLLIN)) 
            {
				client_fd = accept(_socketfd, NULL, NULL);
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
				_poll_fds.push_back(client_poll);
				std::cout << "Welcome to IRC" << std::endl;
				std::cout << "New client connected: fd=" << client_fd << std::endl;
			}
			// Si no es el servidor, entonces es un cliente con datos listos para leer
			else if (_poll_fds[i].revents & POLLIN) 
            {

				char buffer[512];
				ssize_t bytes = recv(_poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

				// if (bytes <= 0)
				// {
				//     int client_fd = _poll_fds[i].fd;
				//     close(client_fd);
				//     _clients_buffer.erase(client_fd);
				//     removeClientBySocket(client_fd);
				//     _poll_fds.erase(_poll_fds.begin() + i);
				//     --i;
				//     continue;
				// }
				/*
							buffer[bytes] = '\0';
							_clients_buffer[_poll_fds[i].fd] += buffer;

							std::string& buf = _clients_buffer[_poll_fds[i].fd];
							size_t pos;
							while ((pos = buf.find("\r\n")) != std::string::npos)
							{
								std::string msg = buf.substr(0, pos);
								buf.erase(0, pos + 2);
								if (msg.empty())
									continue;
								try
								{
									Parsing p(msg);
									std::string echo = msg + "\r\n";
									send(_poll_fds[i].fd, echo.c_str(), echo.size(), 0);
								}
								catch (Parsing::NeedMoreParamsException&)
								{
									std::string err = "461 :Not enough parameters\r\n";
									send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
								}
								catch (Parsing::TooManyParamsException&) {}
								catch (Parsing::UnknownCommandException&)
								{
									std::string err = "421 :Unknown command\r\n";
									send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
								}
							}*/

							// ARREGLAR IDENTACION Y VER SI FUNCIONA LO DEL BUFFER ACUMULATIVO PARA COMANDOS PARTIDOS EN VARIAS LECTURAS. SI FUNCIONA, QUITAR LOS COMENTARIOS DE ARRIBA Y BORRAR ESTE CODIGO DE ABAJO.
								// lectura
					// char buffer1[512];
					// ssize_t bytes1 = recv(_poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

				                                // Si no envia nada o falla, se desconecta y elimina.
				    if (bytes <= 0) 
				    {
							int client_fd = _poll_fds[i].fd;
							close(client_fd);
							removeClientBySocket(client_fd);
							_poll_fds.erase(_poll_fds.begin() + i);
							--i;
							continue;
					}
					
					buffer[bytes] = '\0';
					std::string msg(buffer);
				
					if (!msg.empty() && msg[msg.size() -1] == '\n')
						msg.erase(msg.size() - 1); 
					if (!msg.empty() && msg[msg.size() - 1] == '\r')
						msg.erase(msg.size() - 1);
					try
					{
						Parsing	p(msg, this, getClientBySocket(client_fd));
						//p.parse();
						//logica de ejecutar el comand
						std::string echo = msg + "\r\n";
						send(_poll_fds[i].fd, msg.c_str(), msg.size(), 0); //echo temp
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
						catch(Parsing::MayNotReRegisterException&)
						{
							std::string err = "462: May not reregister\r\n";
							send(_poll_fds[i].fd, err.c_str(), err.size(), 0);
						}
						//std::cout << "[recv] " << msg << std::endl; 
			}
		}
	}
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