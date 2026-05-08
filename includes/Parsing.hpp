#ifndef PARSING_H
#define PARSING_H

#include <algorithm>
#include <cctype>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <map>
#include "Tokenizer.hpp"
#include "Server.hpp"

/*PRIVMSG <msgtarget> <text to be sent>
PRIVMSG is the command.

<msgtarget> can be:

a channel (#cpp, #42, etc.)

a nickname (Andie)

<text to be sent> is the message.

In IRC, if the message starts with :, it means 
“take the rest of the line as one parameter” 
(so spaces are allowed).
*/

enum mode {Mandatory, Optional, List, ListOptional, MultiOptional, Special};

class Parsing
{
	private:

		std::string command; // irc command (NICK, USER, JOIN…)
		Tokenizer inputTokenizer; // splits the input into tokens
		std::vector<std::string> tokens; // holds tokens from the input
		std::string	temppassword;
		unsigned int current; // index of the current token being parsed
		Server	*serv;
		Client	*_client;
		std::map<std::string, std::string> args; // stores named single arguments
		std::map<std::string, std::list<std::string> > args_lists; // stores list arguments

        /*we can store both single and list arguments example:
        NICK <nickname>
        USER <username> <hostname> <servername> :<realname>
        */

		void command_to_upper(std::string & command); // normalize command to uppercase ej: privmsg -> PRIVMSG
	public:

		Parsing(std::string raw_content, Server *, Client *); // takes a string like: NICK <nickname>
		virtual ~Parsing();
		std::string get_current_token();
		bool set_current_arg(std::string arg_name);
		bool set_current_arg_list(std::string arg_name);
		void setTemppassword(std::string);
		void parse();
		void move();
		void exec();
		void pass();
		void nick();
		void user();
		void join();
		void privmsg();
		void kick();
		void invite();
		void topic();
		void modeCmd();

		class InvalidNickException : public std::exception
		{
			public:
			virtual const char* what() const throw();
		};

		class NotRegisteredException : public std::exception
		{
			public:
			virtual const char* what() const throw();
		};

		class NicknameInUseException : public std::exception
		{
			public:
			virtual const char* what() const throw();
		};

		class TooManyParamsException : public std::exception
		{
			public:
			virtual const char* what() const throw();
		};

		class NeedMoreParamsException : public std::exception
		{
			public:
			virtual const char* what() const throw();
		};

		class WrongPasswordException : public std::exception
		{
			public :
			virtual const char *what() const throw();
		};

		class MayNotReRegisterException : public std::exception
		{
			public:
			virtual const char* what() const throw();
		};

		class UnknownCommandException : public std::exception
		{
			public:
			virtual const char* what() const throw();
		};
		void parse_no_arg(); // commands like PING
		void parse_simple(); // commands with fixed arguments (NICK, USER).
		void parse_mode(); // special syntax like MODE #cpp +i.
		std::string get_command();
		void parse_complex(); // multi-argument commands like PRIVMSG
		bool set_current_arg(std::string arg_name, mode arg_type);
		std::list<std::string> get_rest_tokens(std::string current_token);
		std::list<std::string> arg_to_list(const std::string& current_token);
		std::string get(std::string arg_name);
		std::list<std::string> get_list(std::string arg_name);
		bool has_arg(std::string arg_name);
		std::vector<std::string> get_tokens();
		bool has_list(std::string arg_name);
};

/*
client sends: PRIVMSG #cpp :Hello world!

parsing constructor gets raw string
tokenizer splits it into:
"PRIVMSG"  "#cpp"  "Hello world!"

command = "PRIVMSG"

parse_simple() ensures arguments exist

stored in maps:
args["target"] = "#cpp"
args["message"] = "Hello world!"
*/

#endif