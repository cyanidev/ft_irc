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

	//añadir a poll()
}

void Server::start()
{

}