#include "Server.hpp"

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
				int client_fd = accept(_socketfd, NULL, NULL);
				if (client_fd < 0)
					continue;

				// modo no bloqueante
				fcntl(client_fd, F_SETFL, O_NONBLOCK);

                                // se registra el nuevo cliente en poll_fds, para que poll() esté atento a lo que envía ese cliente.
				pollfd client_poll;
				client_poll.fd = client_fd;
				client_poll.events = POLLIN;
				_poll_fds.push_back(client_poll);
			}
			// Si no es el servidor, entonces es un cliente con datos listos para leer
			else if (_poll_fds[i].revents & POLLIN) 
            {
                                // lectura
				char buffer[512];
				ssize_t bytes = recv(_poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
				
                                // Si no envia nada o falla, se desconecta y elimina.
                if (bytes <= 0) 
                {
					close(_poll_fds[i].fd);
					_poll_fds.erase(_poll_fds.begin() + i);
					--i;
					continue;
				}

                                // Convertir mensage a string,
				buffer[bytes] = '\0';
				std::string msg(buffer);
				std::cout << "[recv] " << msg; 
				send(_poll_fds[i].fd, msg.c_str(), msg.size(), 0);
			}
		}
	}
}

int Server::getSocketFD() const 
{
    return _socketfd;
}

Server::~Server() {};