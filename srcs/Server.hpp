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

		std::vector<class channel *>	channels;

	
	public:

		Server(const int &port, const std::string &password);
		~Server();

		void socketInit();
		void start();

		// Handle operations
		void	handleJoin(const std::string& name, const std::string& topic, class client* creator);
		void	handleKick(const std::string& name, class client* user);
		void	handleInvite(const std::string& name, class client* user);
		void	handleTopic(const std::string& name, const std::string& topic);
		void	handleMode(const std::string& name, const std::string& mode, bool enable);
		void	handleMode(const std::string& channel, const std::string& mode, int number);

};

#endif