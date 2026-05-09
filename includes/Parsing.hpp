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

enum mode {Mandatory, Optional, List, ListOptional, MultiOptional, Special};

class Parsing
{
	private:

		std::string command;
		Tokenizer inputTokenizer;
		std::vector<std::string> tokens;
		std::string temppassword;
		unsigned int current;
		Server  *serv;
		Client  *_client;
		std::map<std::string, std::string> args;
		std::map<std::string, std::list<std::string> > args_lists;

		void command_to_upper(std::string &command);

	public:

		Parsing(std::string raw_content, Server *, Client *);
		virtual ~Parsing();

		// Parsing internals
		std::string             get_current_token();
		bool                    set_current_arg(std::string arg_name, mode arg_type);
		bool                    set_current_arg(std::string arg_name);      // sobrecarga original
		bool                    set_current_arg_list(std::string arg_name);
		void                    setTemppassword(std::string);
		void                    parse();
		void                    parse_no_arg();
		void                    parse_simple();
		void                    parse_complex();
		void                    parse_mode();
		void                    move();

		// Accessors
		std::string             get_command();
		std::string             get(std::string arg_name);
		std::list<std::string>  get_list(std::string arg_name);
		std::vector<std::string> get_tokens();
		bool                    has_arg(std::string arg_name);
		bool                    has_list(std::string arg_name);
		std::list<std::string>  get_rest_tokens(std::string current_token);
		std::list<std::string>  arg_to_list(const std::string &current_token);

		// Ejecutor
		void exec();

		// Comandos IRC originales
		void pass();
		void nick();
		void user();
		void join();
		void privmsg();
		void kick();
		void invite();
		void topic();
		void modeCmd();

		// Comandos IRC nuevos
		void part();
		void quit();
		void ping();
		void pong();

		// Excepciones
		class InvalidNickException : public std::exception {
			public: virtual const char* what() const throw();
		};
		class NotRegisteredException : public std::exception {
			public: virtual const char* what() const throw();
		};
		class NicknameInUseException : public std::exception {
			public: virtual const char* what() const throw();
		};
		class TooManyParamsException : public std::exception {
			public: virtual const char* what() const throw();
		};
		class NeedMoreParamsException : public std::exception {
			public: virtual const char* what() const throw();
		};
		class WrongPasswordException : public std::exception {
			public: virtual const char* what() const throw();
		};
		class MayNotReRegisterException : public std::exception {
			public: virtual const char* what() const throw();
		};
		class UnknownCommandException : public std::exception {
			public: virtual const char* what() const throw();
		};
};

#endif