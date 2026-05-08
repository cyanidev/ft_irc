
#include "Server.hpp"
#include "../includes/Client.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <sstream>

static std::string prefix(Client* c)
{
	return ":" + c->getNickname() + "!" + c->getUsername() + "@localhost";
}

Channel* Server::findChannel(const std::string& name)
{
	std::string search = (name[0] == '#') ? name : "#" + name;
	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (channels[i]->getName() == search)
			return channels[i];
	}
	return NULL;
}

Client* Server::findClientByNickname(const std::string& nick)
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i]->getNickname() == nick)
			return clients[i];
	}
	return NULL;
}

void	Server::handleJoin(Client* client, const std::string& channelName, const std::string& key)
{
	std::string name = (channelName[0] == '#') ? channelName : "#" + channelName;

    Channel* chan = findChannel(name);

    if (!chan)
	{
        // canal nuevo: el cliente es el creador y auto operador
        chan = new Channel(name.substr(1), "", client);
        channels.push_back(chan);
        client->addChannel(chan);
    } 
	else 
	{
        // canal existente: validaciones
        if (chan->isBanned(client)) {
            client->sendMessage(":server 474 " + client->getNickname() + " " + name + " :Cannot join channel (+b)\r\n");
            return;
        }
        if (chan->isInviteOnly() && !chan->isInvited(client)) {
            client->sendMessage(":server 473 " + client->getNickname() + " " + name + " :Cannot join channel (+i)\r\n");
            return;
        }
        if (chan->isKeyRequired() && key != chan->getKey()) {
            client->sendMessage(":server 475 " + client->getNickname() + " " + name + " :Cannot join channel (+k)\r\n");
            return;
        }
        if (chan->getMaxUsers() > 0 && chan->getCurrentUsers() >= chan->getMaxUsers()) {
            client->sendMessage(":server 471 " + client->getNickname() + " " + name + " :Cannot join channel (+l)\r\n");
            return;
        }
        if (chan->isUser(client))
			return; // ya está en el canal
		chan->addUser(client);
		client->addChannel(chan);
	}
	chan->broadcastMessage(prefix(client) + " JOIN " + name + "\r\n", NULL);
	if (chan->getTopic().empty()) 
	{
		client->sendMessage(":server 331 " + client->getNickname() + " " + name + " :No topic is set\r\n");
	}
	else
	{
		client->sendMessage(":server 332 " + client->getNickname() + " " + name + " :" + chan->getTopic() + "\r\n");
	}
	std::string namreply = ":server 353 " + client->getNickname() + " = " + name + " :";
    const std::vector<Client*>& users = chan->getUsers();
    for (size_t i = 0; i < users.size(); ++i)
	{
        if (chan->isOperator(users[i]))
            namreply += "@";
        namreply += users[i]->getNickname();
        if (i + 1 < users.size())
            namreply += " ";
    }
    namreply += "\r\n";
    client->sendMessage(namreply);
    client->sendMessage(":server 366 " + client->getNickname() + " " + name + " :End of /NAMES list\r\n");
}

void	Server::handleKick(Client* oper, Client* target, const std::string& channelName, const std::string& reason)
{
	Channel* chan = findChannel(channelName);

	if (!chan)
	{
		oper->sendMessage(":server 403 " + oper->getNickname() + " " + channelName + " :No such channel\r\n");
		return;
    }
    if (!chan->isUser(oper))
	{
        oper->sendMessage(":server 442 " + oper->getNickname() + " " + chan->getName() + " :You're not on that channel\r\n");
        return;
    }
    if (!chan->isOperator(oper))
	{
        oper->sendMessage(":server 482 " + oper->getNickname() + " " + chan->getName() + " :You're not channel operator\r\n");
        return;
    }
    if (!chan->isUser(target))
	{
        oper->sendMessage(":server 441 " + oper->getNickname() + " " + target->getNickname() + " " + chan->getName() + " :They aren't on that channel\r\n");
        return;
    }

    std::string msg = prefix(oper) + " KICK " + chan->getName() + " " + target->getNickname() + " :" + reason + "\r\n";
    chan->broadcastMessage(msg, NULL); // todos ven el kick, incluido el kickeado
    chan->removeUser(target);
    target->removeChannel(chan);
}


void	Server::handleInvite(Client* oper, Client* target, const std::string& channelName)
{
    Channel* chan = findChannel(channelName);

	if (!chan)
		{
		oper->sendMessage(":server 403 " + oper->getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}
	if (!chan->isUser(oper))
		{
		oper->sendMessage(":server 442 " + oper->getNickname() + " " + chan->getName() + " :You're not on that channel\r\n");
		return;
	}
	if (chan->isInviteOnly() && !chan->isOperator(oper))
		{
		oper->sendMessage(":server 482 " + oper->getNickname() + " " + chan->getName() + " :You're not channel operator\r\n");
		return;
	}
	if (chan->isUser(target))
		{
		oper->sendMessage(":server 443 " + oper->getNickname() + " " + target->getNickname() + " " + chan->getName() + " :is already on channel\r\n");
		return;
	}

    chan->addInvited(target);
    oper->sendMessage(":server 341 " + oper->getNickname() + " " + target->getNickname() + " " + chan->getName() + "\r\n");
    target->sendMessage(prefix(oper) + " INVITE " + target->getNickname() + " " + chan->getName() + "\r\n");
}


void	Server::handleTopic(Client* client, const std::string& channelName, const std::string& topic, bool hasTopic)
{
	Channel* chan = findChannel(channelName);

	if (!chan)
	{
		client->sendMessage(":server 403 " + client->getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}
	if (!chan->isUser(client))
	{
		client->sendMessage(":server 442 " + client->getNickname() + " " + chan->getName() + " :You're not on that channel\r\n");
		return;
	}

    // solo consulta
    if (!hasTopic)
	{
		if (chan->getTopic().empty())
			client->sendMessage(":server 331 " + client->getNickname() + " " + chan->getName() + " :No topic is set\r\n");
		else
			client->sendMessage(":server 332 " + client->getNickname() + " " + chan->getName() + " :" + chan->getTopic() + "\r\n");
		return;
	}

    // cambio de topic: si +t, solo ops
    if (chan->isRestrictedTopic() && !chan->isOperator(client))
	{
		client->sendMessage(":server 482 " + client->getNickname() + " " + chan->getName() + " :You're not channel operator\r\n");
		return;
	}

    chan->setTopic(topic);
    chan->broadcastMessage(prefix(client) + " TOPIC " + chan->getName() + " :" + topic + "\r\n", NULL);
}

void	Server::handleMode(Client* oper, const std::string& channelName, char mode, bool enable, const std::string& param)
{
	Channel* chan = findChannel(channelName);

	if (!chan)
		{
		oper->sendMessage(":server 403 " + oper->getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}
	if (!chan->isUser(oper))
		{
		oper->sendMessage(":server 442 " + oper->getNickname() + " " + chan->getName() + " :You're not on that channel\r\n");
		return;
	}
	if (!chan->isOperator(oper))
		{
		oper->sendMessage(":server 482 " + oper->getNickname() + " " + chan->getName() + " :You're not channel operator\r\n");
		return;
	}

    std::string modeStr = enable ? "+" : "-";
    modeStr += mode;
    std::string modeMsg = prefix(oper) + " MODE " + chan->getName() + " " + modeStr;

    switch (mode)
	{
        case 'i':
            chan->setInviteOnly(enable);
            break;
        case 't':
            chan->setRestrictedTopic(enable);
            break;
        case 'k':
            chan->setKeyRequired(enable);
            if (enable && !param.empty())
			{
                chan->setKey(param);
                modeMsg += " " + param;
            }
			else if (!enable)
			{
                chan->setKey("");
            }
            break;
        case 'o':
		{
            Client* target = findClientByNickname(param);
            if (!target || !chan->isUser(target))
			{
                oper->sendMessage(":server 441 " + oper->getNickname() + " " + param + " " + chan->getName() + " :They aren't on that channel\r\n");
                return;
            }
            if (enable)
                chan->addOperator(target);
            else
                chan->removeOperator(target);
            modeMsg += " " + param;
            break;
        }
        case 'l':
            if (enable)
			{
                int limit = std::atoi(param.c_str());
                if (limit <= 0)
				{
                    oper->sendMessage(":server 461 " + oper->getNickname() + " MODE :Invalid limit\r\n");
                    return;
                }
                chan->setMaxUsers(limit);
                std::ostringstream oss;
                oss << limit;
                modeMsg += " " + oss.str();
            }
			else
			{
                chan->setMaxUsers(0);
            }
            break;
        default:
            oper->sendMessage(":server 472 " + oper->getNickname() + " " + mode + " :Unknown mode\r\n");
            return;
    }

    modeMsg += "\r\n";
    chan->broadcastMessage(modeMsg, NULL);
}


