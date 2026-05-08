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
# include <map>
#include "Channel.hpp"
#include "Client.hpp"



class Server
{
	protected:

		int _port;
		int _socketfd;
		std::string _password;

		std::vector<struct pollfd> _poll_fds;
		std::vector<Channel*> channels;
		std::vector<Client*>  clients;
		std::map<int, std::string> _clients_buffer;

		void socketInit();
		void removeClientBySocket(int socket);
		
		
	public:
		Server(const int &port, const std::string &password);
		~Server();

		void 		start();
		int			getSocketFD() const;
		std::string	get_password() const;

		//clientss y canales
		Client*		getClientBySocket(int socket);
		Client*		findClientByNickname(const std::string& nick);
		bool		findClientByNick(std::string& nick);
		bool		findClientByUser(std::string user);
		Channel*	findChannel(const std::string& name);

		// Handle operations
		void handleJoin(Client* client, const std::string& channelName, const std::string& key);
        void handleKick(Client* oper, Client* target, const std::string& channelName, const std::string& reason);
        void handleInvite(Client* oper, Client* target, const std::string& channelName);
        void handleTopic(Client* client, const std::string& channelName, const std::string& topic, bool hasTopic);
        void handleMode(Client* oper, const std::string& channelName, char mode, bool enable, const std::string& param);
};

#endif