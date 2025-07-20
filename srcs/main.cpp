#include <iostream>
#include <stdlib>

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Error in args, try: ./ircserv <port> <psswrd>" << std::endl;
		return (1);
	}
	int port = std::atoi(argv[1]); //check 1024 to 49151
	std::string pswrd = argv[2];
	
	return (0);
}
