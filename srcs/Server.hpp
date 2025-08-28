#ifndef SERVER_HPP
#define SERVER_HPP

# include <string>
# include <vector>
# include <stdexcept>
# include <cstring>      // para memset
# include <unistd.h>     // para close
# include <fcntl.h>      // para fcntl
# include <netinet/in.h> // para sockaddr_in, htons, etc.
# include <sys/socket.h> // para socket, bind, listen, accept
# include <poll.h>       // para poll, pollfd


class Server
{
	private:

		int _port;
		int _socketfd;

		std::string _password;
		std::vector<struct pollfd> _poll_fds;

		void socketInit();
	
	public:

		Server(const int &port, const std::string &password);
		~Server();

		
		void start();
		int getSocketFD() const;

};

#endif