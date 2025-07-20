#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>

class Server
{
	private:

		int _port;
		int _socketfd;

		std::string _password;

	
	public:

		Server(const int &port, const std::string &password);
		~Server();

		void socketInit();
		void start();

};

#endif