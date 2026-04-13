#include "Parsing.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>
#include "Tokenizer.hpp"

#define NUMBER_CMD 24

std::string commands[NUMBER_CMD] = {"ADMIN", "INFO", "VERSION", "USERS", "NICK", "PRIVMSG", "USER", "QUIT", "JOIN", "LIST", "NAMES", "SUMMON", "KICK", "PART", "MODE", "CAP", "PASS", "WHOIS", "INVITE", "TOPIC", "PING", "PONG"};
std::string params[NUMBER_CMD][10] = {{"target"}, {"target"}, {}, {}, {"nickname"}, {"msgtarget", "text to be sent"}, {"user", "mode", "unused", "realname"}, {"Quit Message"}, {"channel", "key"}, {"channel"}, {"channel", "target"}, {"user", "target", "channel"}, {"channel", "user", "comment"}, {"channel", "Part Message"}, {"target", "modestring", "mode arguments"}, {"a"}, {"password"}, {"channel", "user", "comment"}, {"nickname", "channel"}, {"channel", "topic"}, {"token"}, {"token"}};

mode params_states[NUMBER_CMD][10] = {{Optional}, {Optional}, {}, {}, {Mandatory}, {Mandatory, Optional}, {Mandatory, Mandatory, Mandatory, Mandatory}, {Optional}, {List, ListOptional}, {ListOptional}, {ListOptional, Optional}, {Mandatory, Optional, Optional}, {List, List, Optional}, {List, Optional}, {Mandatory, Optional, MultiOptional}, {Optional}, {Optional}, {Mandatory, List, Optional}, {Mandatory, Mandatory}, {Mandatory, Optional}, {Mandatory}, {Mandatory}};

// Helper functions
bool is_in_array(const std::string& value, const std::string array[], size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (array[i] == value) {
            return true;
        }
    }
    return false;
}

unsigned int get_array_index(const std::string& value, const std::string array[], size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (array[i] == value) {
            return i;
        }
    }
    return -1; // or throw exception, but assuming it's found
}

Parsing::Parsing(std::string raw_content) : inputTokenizer(Tokenizer(raw_content)), current(0)
{
	inputTokenizer.tokenize();
	tokens = inputTokenizer.get_tokens();
	if (tokens.size() == 0)
	{
		throw Parsing::UnknownCommandException();
	}
	command = tokens[0];
	command_to_upper(command);
	move(); // skip past the command, so current points at first argument
}

void Parsing::command_to_upper(std::string & command)
{
    for (std::string::iterator it = command.begin(); it != command.end(); it++)
    {
        *it = toupper(*it);
    }
}

// check if the command is in the predefined commands array
// and if the number of arguments is valid
void Parsing::parse()
{
	if (!is_in_array(command, commands, NUMBER_CMD))
	{
		throw Parsing::UnknownCommandException();
	}

	if (is_in_array(command, commands, NUMBER_CMD))
	{
		try
		{
			parse_complex();
		}
		catch (std::out_of_range const &e)
		{
			throw NeedMoreParamsException();
		}
	}
}

// for commands with no arguments like ping pong
void Parsing::parse_no_arg()
{
	if (tokens.size() > 1)
	{
		throw Parsing::TooManyParamsException();
	}
	return;
}

// parameter spec for complex commands, iterates through the params
//trying to match it with the tokens if missing throws, if too many tokens throws
void Parsing::parse_complex()
{
	unsigned int command_index = get_array_index(command, commands, NUMBER_CMD);
	unsigned int i = 0;
	std::string current_param = params[command_index][i] ;
	mode current_type = params_states[command_index][i] ;

	while (!current_param.empty())
	{
		if (!set_current_arg(current_param, current_type))
		{
			throw NeedMoreParamsException();
		}
		move();
		i++;
		current_param = params[command_index][i] ;
		current_type = params_states[command_index][i] ;
	}
	if (tokens.size() > i + 1 && command != "MODE")
	{
		throw TooManyParamsException();
	}
}

std::string Parsing::get_current_token()
{
	if (current >= tokens.size())
	{
		throw std::out_of_range("No more tokens");
	}
	return (tokens[current]);
}




// splits on commas into a list
std::list<std::string> Parsing::arg_to_list(const std::string& current_token)
{
    std::list<std::string> args_list;
    size_t start = 0;
    size_t end = 0;

    while ((end = current_token.find(',', start)) != std::string::npos)
    {
        args_list.push_back(current_token.substr(start, end - start));
        start = end + 1;
    }
    args_list.push_back(current_token.substr(start));
    return args_list;
}

//if arg is a list (JOIN #a,#b), split by commas
// //if arg is MultiOptional (like MODE with multiple settings), 
// consume the rest of the tokens otherwise, store a single string
bool Parsing::set_current_arg(std::string arg_name, mode arg_type)
{
	try
	{
		std::string current_token = get_current_token();
		if (arg_type == List || arg_type == ListOptional)
		{
			args_lists[arg_name] = arg_to_list(current_token);
		}
		else if (arg_type == MultiOptional)
		{
			args_lists[arg_name] = get_rest_tokens(current_token);
		}
		else
		{
			args[arg_name] = current_token;
		}
	}
	catch (std::out_of_range const &e)
	{
		if ( arg_type == Mandatory || arg_type == List )
		{
			return ( false );
		}
		return ( true );
	}
	return ( true );
}

// for commands that can take unlimited trailing args
//  (like MODE with multiple mode letters)
std::list<std::string> Parsing::get_rest_tokens( std::string current_token )
{
	std::list<std::string> arguments;
	std::string token;
	bool unfinished = true;

	arguments.push_back( current_token );
	while ( unfinished )
	{
		try
		{
			move();
			token = get_current_token();
			arguments.push_back(token);
		}
		catch(std::out_of_range & e)
		{
			(void)e;
			unfinished = false;
		}
	}
	return arguments;
}

bool Parsing::set_current_arg_list(std::string arg_name)
{
    try
    {
        std::string current_token = get_current_token();
        std::list<std::string> arg_list;
        size_t start = 0, end = 0;

        while ((end = current_token.find(',', start)) != std::string::npos)
        {
            arg_list.push_back(current_token.substr(start, end - start));
            start = end + 1;
        }
        arg_list.push_back(current_token.substr(start));
        args_lists[arg_name] = arg_list;
        return true;
    }
    catch (std::out_of_range const &e)
    {
        return false;
    }
}

void Parsing::move()
{
	current++;
}

std::string Parsing::get_command()
{
	return (command);
}

std::string Parsing::get(std::string arg_name)
{
	return (args[arg_name]);
}

std::list<std::string> Parsing::get_list(std::string arg_name)
{
	return (args_lists[arg_name]);
}

std::vector<std::string> Parsing::get_tokens(void)
{
	return (tokens);
}

bool Parsing::has_arg(std::string arg_name)
{
	if (args.count( arg_name ) == 0)
	{
		return (false);
	}
	return (true);
}

bool Parsing::has_list(std::string arg_name)
{
	if (args_lists.count( arg_name ) == 0)
	{
		return (false);
	}
	return (true);
}


Parsing::~Parsing() {}

const char* Parsing::NeedMoreParamsException::what() const throw()
{
	return ("Not enough parameters provided");
}

const char* Parsing::TooManyParamsException::what() const throw()
{
	return ("Too many parameters provided");
}

const char* Parsing::UnknownCommandException::what() const throw()
{
	return ("Unknown command");
}