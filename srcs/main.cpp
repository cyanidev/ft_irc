#include <iostream>
#include "Server.hpp"

int main(int argc, char** argv)
{
	(void)argv;
	if (argc != 3)
	{
		std::cout << "Error in args, try: ./ircserv <port> <psswrd>" << std::endl;
		return (1);
	}
	try
    {
        Server server(av[1], argv[2]); // Se va a modificar antes para que sea un it
		server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

	return (0);
}
