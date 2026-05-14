#ifndef SERVER_HPP
#define SERVER_HPP

# include <string>
# include <vector>
# include <cstdlib>
#include <iostream>
# include <stdexcept>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <csignal>
# include <poll.h>
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
		void removeChannel(Channel* chan); // elimina canal del vector y libera memoria

	public:
		Server(const int &port, const std::string &password);
		~Server();

		void 		start();
		int			getSocketFD() const;
		std::string	get_password() const;

		// Clientes y canales
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
		void handlePart(Client* client, const std::string& channelName, const std::string& reason); // nuevo
		void handleQuit(Client* client, const std::string& reason);                                 // nuevo
};

#endif