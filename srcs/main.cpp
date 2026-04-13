#include "Server.hpp"

int parsePort(char* arg)
{
    try
    {
		char *end;
        int port = strtol(arg, &end, 10);
        if (port < 1024 || port > 49151)
        {
            throw std::out_of_range("Error: Port must be between 1024 and 49151");
        }
        return port;
    }
    catch (const std::invalid_argument&)
    {
        throw std::invalid_argument("Error: Port must be a valid number.");
    }
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <psswrd>" << std::endl;
		return 1;
	}
	try
	{
		int port = parsePort(argv[1]);
		std::string pswrd = argv[2];

		Server server(port, pswrd);
		server.start();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

