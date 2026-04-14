#ifndef SERVER_HPP
#define SERVER_HPP

# include <string>
# include <vector>
# include <cstdlib>
#include <iostream>
# include <stdexcept>
# include <cstring>      // para memset
# include <unistd.h>     // para close
# include <fcntl.h>      // para fcntl
# include <netinet/in.h> // para sockaddr_in, htons, etc.
# include <sys/socket.h> // para socket, bind, listen, accept
# include <poll.h>       // para poll, pollfd
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"


class Server
{
	private:

		int _port;
		int _socketfd;
		std::string _password;

		std::vector<struct pollfd> _poll_fds;
		std::vector<Channel*> channels;
		std::vector<Client*>  clients;

		void socketInit();
		Client* getClientBySocket(int socket);
		void removeClientBySocket(int socket);
	
	public:

		Server(const int &port, const std::string &password);
		~Server();

		
		void start();
		int getSocketFD() const;

		// Handle operations
		void	handleJoin(const std::string& name, const std::string& topic, class Client* creator);
		void	handleKick(const std::string& name, class Client* user);
		void	handleInvite(const std::string& name, class Client* user);
		void	handleTopic(const std::string& name, const std::string& topic);
		void	handleMode(const std::string& name, const std::string& mode, bool enable);
		void	handleMode(const std::string& channel, const std::string& mode, int number);

};

#endif