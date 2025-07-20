#include <iostream>

int main(int argc, char** argv)
{
	(void)argv;

	
	if (argc != 3)
	{
		std::cout << "Error in args, try: ./ircserv <port> <psswrd>" << std::endl;
		return (1);
	}
	return (0);
}
