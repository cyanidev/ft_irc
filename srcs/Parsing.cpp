#include "Parsing.hpp"
#include "Tokenizer.hpp"

#define NUMBER_CMD 13

std::string commands[NUMBER_CMD] = {
	"PASS", "NICK", "USER", "JOIN",
	"PRIVMSG", "KICK", "INVITE",
	"TOPIC", "MODE", "PART", "QUIT",
	"PING", "PONG"};

std::string params[NUMBER_CMD][10] = {
	{"password"},								// PASS
	{"nickname"},								// NICK
	{"user", "mode", "unused", "realname"},		// USER
	{"channel", "key"},							// JOIN
	{"msgtarget", "text"},						// PRIVMSG
	{"channel", "user", "comment"},				// KICK
	{"nickname", "channel"},					// INVITE
	{"channel", "topic"},						// TOPIC
	{"target", "modestring", "mode arguments"}, // MODE
	{"channel", "message"},						// PART
	{"message"},								// QUIT
	{"server"},									// PING
	{"server"},									// PONG
};

mode params_states[NUMBER_CMD][10] = {
	{Mandatory},								  // PASS
	{Mandatory},								  // NICK
	{Mandatory, Mandatory, Mandatory, Mandatory}, // USER
	{List, ListOptional},						  // JOIN
	{Mandatory, Mandatory},						  // PRIVMSG
	{List, List, Optional},						  // KICK
	{Mandatory, Mandatory},						  // INVITE
	{Mandatory, Optional},						  // TOPIC
	{Mandatory, Optional, MultiOptional},		  // MODE
	{List, Optional},							  // PART
	{Optional},									  // QUIT
	{Mandatory},								  // PING
	{Optional},									  // PONG
};

// Helper functions
bool is_in_array(const std::string &value, const std::string array[], size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		if (array[i] == value)
		{
			return true;
		}
	}
	return false;
}

unsigned int get_array_index(const std::string &value, const std::string array[], size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		if (array[i] == value)
		{
			return i;
		}
	}
	return -1;
}

Parsing::Parsing(std::string raw_content, Server *temp, Client *temp2) : inputTokenizer(Tokenizer(raw_content)), current(0), serv(temp), _client(temp2)
{
	inputTokenizer.tokenize();
	tokens = inputTokenizer.get_tokens();
	if (tokens.size() == 0)
	{
		throw Parsing::UnknownCommandException();
	}
	command = tokens[0];
	command_to_upper(command);
	move();
	parse();
	exec();
}

void Parsing::command_to_upper(std::string &command)
{
	for (std::string::iterator it = command.begin(); it != command.end(); it++)
	{
		*it = toupper(*it);
	}
}

void Parsing::parse()
{
	if (!is_in_array(command, commands, NUMBER_CMD))
	{
		throw Parsing::UnknownCommandException();
	}
	else
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

void Parsing::parse_no_arg()
{
	if (tokens.size() > 1)
	{
		throw Parsing::TooManyParamsException();
	}
	return;
}

void Parsing::parse_complex()
{
	unsigned int command_index = get_array_index(command, commands, NUMBER_CMD);
	unsigned int i = 0;
	std::string current_param = params[command_index][i];
	mode current_type = params_states[command_index][i];

	while (!current_param.empty())
	{
		if (!set_current_arg(current_param, current_type))
		{
			throw NeedMoreParamsException();
		}
		move();
		i++;
		current_param = params[command_index][i];
		current_type = params_states[command_index][i];
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

std::list<std::string> Parsing::arg_to_list(const std::string &current_token)
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
		if (arg_type == Mandatory || arg_type == List)
		{
			return (false);
		}
		return (true);
	}
	return (true);
}

std::list<std::string> Parsing::get_rest_tokens(std::string current_token)
{
	std::list<std::string> arguments;
	std::string token;
	bool unfinished = true;

	arguments.push_back(current_token);
	while (unfinished)
	{
		try
		{
			move();
			token = get_current_token();
			arguments.push_back(token);
		}
		catch (std::out_of_range &e)
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
	if (args.count(arg_name) == 0)
	{
		return (false);
	}
	return (true);
}

void Parsing::setTemppassword(std::string temp)
{
	temppassword = temp;
}

bool Parsing::has_list(std::string arg_name)
{
	if (args_lists.count(arg_name) == 0)
	{
		return (false);
	}
	return (true);
}

Parsing::~Parsing() {}

const char *Parsing::NotRegisteredException::what() const throw()
{
	return ("Client not registered");
}

const char *Parsing::NicknameInUseException::what() const throw()
{
	return ("Nick name in use");
}

const char *Parsing::MayNotReRegisterException::what() const throw()
{
	return ("May not reregistered");
}

const char *Parsing::WrongPasswordException::what() const throw()
{
	return ("Wrong Password");
}

const char *Parsing::NeedMoreParamsException::what() const throw()
{
	return ("Not enough parameters provided");
}

const char *Parsing::TooManyParamsException::what() const throw()
{
	return ("Too many parameters provided");
}

const char *Parsing::UnknownCommandException::what() const throw()
{
	return ("Unknown command");
}

const char *Parsing::InvalidNickException::what() const throw()
{
	return ("Invalid Nickname");
}

void Parsing::exec()
{
	unsigned int command_index = get_array_index(command, commands, NUMBER_CMD);
	switch(command_index)
	{
		case 0:  
			pass();
			break;
		case 1:  
			nick();
			break;
		case 2:
			user();
			break;
		case 3:  
			join();    
			break;
		case 4:  
			privmsg(); 
			break;
		case 5:  
			kick();    
			break;
		case 6:  
			invite();  
			break;
		case 7:  
			topic();   
			break;
		case 8:  
			modeCmd(); 
			break;
		case 9:  
			part();    
			break;
		case 10: 
			quit();    
			break;
		case 11: 
			ping();    
			break;
		case 12: 
			pong();    
			break;
		default:
			throw UnknownCommandException();
	}
}

void Parsing::pass()
{
	if (_client->isRegistered())
		throw MayNotReRegisterException();
	if (tokens[1] == serv->get_password())
		_client->setRegistered(true);
	else
		throw WrongPasswordException();
}

void Parsing::nick()
{
	if (!_client->get_register())
		throw NotRegisteredException();
	if (tokens[1].size() > 9 || tokens[1].find(' ') != std::string::npos
		|| tokens[1].find('@') != std::string::npos || tokens[1].find('!') != std::string::npos
		|| tokens[1].find('#') != std::string::npos || tokens[1].find(':') != std::string::npos
		|| tokens[1].find(',') != std::string::npos || tokens[1] == "")
		throw InvalidNickException();
	for (size_t i = 0; tokens[1].size() > i; i++)
	{
		if (tokens[1][i] < 32 || tokens[1][i] == 127)
			throw InvalidNickException();
	}
	if (_client->getNickname() == "")
	{
		if (serv->findClientByNick(tokens[1]))
			throw NicknameInUseException();
		_client->setNickname(tokens[1]);
	}
	else
	{
		if (tokens[1] == _client->getNickname())
			return;
		if (serv->findClientByNick(tokens[1]))
			throw NicknameInUseException();
		else
			_client->setNickname(tokens[1]);
	}
}

void Parsing::user()
{
	if (!_client->get_register())
		throw NotRegisteredException();
	for (size_t j = 1; 4 > j; j++)
	{
		if (tokens[j].size() > 9 || tokens[j].find(' ') != std::string::npos
		|| tokens[j].find('@') != std::string::npos || tokens[j].find('!') != std::string::npos
		|| tokens[j].find('#') != std::string::npos || tokens[j].find(':') != std::string::npos
		|| tokens[j].find(',') != std::string::npos || tokens[j] == "")
			throw NeedMoreParamsException();
		for (size_t i = 0; tokens[j].size() > i; i++)
		{
			if (tokens[j][i] < 32 || tokens[j][i] == 127)
				throw NeedMoreParamsException();
		}
	}
	if (_client->getUsername() == "")
	{
		_client->setUsername(tokens[1]);
		_client->setRealname(tokens[4]);
		std::string nick = _client->getNickname();
		_client->sendMessage(":server 001 " + nick + " :Welcome to the IRC server " + nick + "\r\n");
		_client->sendMessage(":server 002 " + nick + " :Your host is ircserv\r\n");
		_client->sendMessage(":server 003 " + nick + " :This server was created today\r\n");
		_client->sendMessage(":server 004 " + nick + " ircserv 1.0 o itkol\r\n");
	}
	else
		throw MayNotReRegisterException();
	
}

void Parsing::join()
{
	if (!_client->isRegistered())
		throw NotRegisteredException();

	std::list<std::string> chans = get_list("channel");
	std::list<std::string> keys  = has_list("key") ? get_list("key") : std::list<std::string>();

	std::list<std::string>::iterator chanIt = chans.begin();
	std::list<std::string>::iterator keyIt  = keys.begin();

	while (chanIt != chans.end())
	{
		std::string key = (keyIt != keys.end()) ? *keyIt++ : "";
		serv->handleJoin(_client, *chanIt, key);
		++chanIt;
	}
}

void Parsing::privmsg()
{
	if (!_client->isRegistered())
		throw NotRegisteredException();

	std::string target = get("msgtarget");
	std::string text   = get("text");
	std::string msg    = ":" + _client->getNickname() + "!" + _client->getUsername()
						 + "@localhost PRIVMSG " + target + " :" + text + "\r\n";

	if (!target.empty() && target[0] == '#')
	{
		Channel* chan = serv->findChannel(target);
		if (!chan)
		{
			_client->sendMessage(":server 403 " + _client->getNickname() + " " + target + " :No such channel\r\n");
			return;
		}
		if (!chan->isUser(_client))
		{
			_client->sendMessage(":server 404 " + _client->getNickname() + " " + target + " :Cannot send to channel\r\n");
			return;
		}
		chan->broadcastMessage(msg, _client);
	}
	else
	{
		Client* dest = serv->findClientByNickname(target);
		if (!dest)
		{
			_client->sendMessage(":server 401 " + _client->getNickname() + " " + target + " :No such nick\r\n");
			return;
		}
		dest->sendMessage(msg);
	}
}

void Parsing::kick()
{
	if (!_client->isRegistered())
		throw NotRegisteredException();

	std::list<std::string> chans  = get_list("channel");
	std::list<std::string> users  = get_list("user");
	std::string reason = has_arg("comment") ? get("comment") : _client->getNickname();

	std::list<std::string>::iterator chanIt = chans.begin();
	std::list<std::string>::iterator userIt = users.begin();

	while (chanIt != chans.end() && userIt != users.end())
	{
		Client* target = serv->findClientByNickname(*userIt);
		if (!target)
			_client->sendMessage(":server 401 " + _client->getNickname() + " " + *userIt + " :No such nick\r\n");
		else
			serv->handleKick(_client, target, *chanIt, reason);
		++chanIt;
		++userIt;
	}
}

void Parsing::invite()
{
	if (!_client->isRegistered())
		throw NotRegisteredException();

	std::string nick     = get("nickname");
	std::string chanName = get("channel");

	Client* target = serv->findClientByNickname(nick);
	if (!target)
	{
		_client->sendMessage(":server 401 " + _client->getNickname() + " " + nick + " :No such nick\r\n");
		return;
	}
	serv->handleInvite(_client, target, chanName);
}

void Parsing::topic()
{
	if (!_client->isRegistered())
		throw NotRegisteredException();

	std::string chanName = get("channel");
	bool        hasTopic = has_arg("topic");
	std::string topic    = hasTopic ? get("topic") : "";

	serv->handleTopic(_client, chanName, topic, hasTopic);
}

void Parsing::modeCmd()
{
	if (!_client->isRegistered())
		throw NotRegisteredException();

	std::string target = get("target");

	if (!has_arg("modestring"))
		return;

	std::string modestring = get("modestring");

	std::list<std::string> modeArgs;
	if (has_list("mode arguments"))
		modeArgs = get_list("mode arguments");

	bool enable = true;
	std::list<std::string>::iterator argIt = modeArgs.begin();

	for (size_t i = 0; i < modestring.size(); ++i)
	{
		char c = modestring[i];
		if (c == '+') { enable = true;  continue; }
		if (c == '-') { enable = false; continue; }

		std::string param = "";
		if ((c == 'k' && enable) || c == 'o' || (c == 'l' && enable))
		{
			if (argIt != modeArgs.end())
				param = *argIt++;
		}
		serv->handleMode(_client, target, c, enable, param);
	}
}

// ── Nuevos comandos ────────────────────────────────────────────────────────────

void Parsing::part()
{
	if (!_client->isRegistered())
		throw NotRegisteredException();

	std::list<std::string> chans = get_list("channel");
	std::string reason = has_arg("message") ? get("message") : "";

	for (std::list<std::string>::iterator it = chans.begin(); it != chans.end(); ++it)
	{
		serv->handlePart(_client, *it, reason);
	}
}

void Parsing::quit()
{
	std::string reason = has_arg("message") ? get("message") : "Client quit";
	serv->handleQuit(_client, reason);
}

void Parsing::ping()
{
	std::string server = get("server");
	_client->sendMessage(":server PONG server :" + server + "\r\n");
}

void Parsing::pong()
{
	(void)this;
}