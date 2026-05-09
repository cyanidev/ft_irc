#include "Server.hpp"
#include "../includes/Client.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <sstream>
#include <algorithm>

static std::string prefix(Client* c)
{
	return ":" + c->getNickname() + "!" + c->getUsername() + "@localhost";
}

// ── Utilidades ────────────────────────────────────────────────────────────────

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

// Promueve al primer usuario del canal que no sea ya operador.
// Envía MODE +o al canal para que todos se enteren.
static void promoteNextOperator(Channel* chan)
{
	if (chan->getCurrentUsers() == 0)
		return;

	const std::vector<Client*>& users = chan->getUsers();
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (!chan->isOperator(users[i]))
		{
			chan->addOperator(users[i]);
			std::string modeMsg = ":server MODE " + chan->getName()
								+ " +o " + users[i]->getNickname() + "\r\n";
			chan->broadcastMessage(modeMsg, NULL);
			return;
		}
	}
}

// Elimina el canal del servidor y libera memoria.
void Server::removeChannel(Channel* chan)
{
	std::vector<Channel*>::iterator it = std::find(channels.begin(), channels.end(), chan);
	if (it != channels.end())
	{
		channels.erase(it);
		delete chan;
	}
}

// ── Handlers ──────────────────────────────────────────────────────────────────

void Server::handleJoin(Client* client, const std::string& channelName, const std::string& key)
{
	std::string name = (channelName[0] == '#') ? channelName : "#" + channelName;

	Channel* chan = findChannel(name);

	if (!chan)
	{
		// Canal nuevo: el cliente es creador y auto-operador
		chan = new Channel(name.substr(1), "", client);
		channels.push_back(chan);
		client->addChannel(chan);
	}
	else
	{
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
		client->sendMessage(":server 331 " + client->getNickname() + " " + name + " :No topic is set\r\n");
	else
		client->sendMessage(":server 332 " + client->getNickname() + " " + name + " :" + chan->getTopic() + "\r\n");

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

void Server::handleKick(Client* oper, Client* target, const std::string& channelName, const std::string& reason)
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

	bool wasOperator = chan->isOperator(target);

	std::string msg = prefix(oper) + " KICK " + chan->getName() + " " + target->getNickname() + " :" + reason + "\r\n";
	chan->broadcastMessage(msg, NULL);
	chan->removeUser(target);
	target->removeChannel(chan);

	// Si el expulsado era el único operador, promover al siguiente usuario
	if (wasOperator && chan->getCurrentUsers() > 0 && !chan->hasOperator())
		promoteNextOperator(chan);

	if (chan->getCurrentUsers() == 0)
		removeChannel(chan);
}

void Server::handleInvite(Client* oper, Client* target, const std::string& channelName)
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

void Server::handleTopic(Client* client, const std::string& channelName, const std::string& topic, bool hasTopic)
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

	if (!hasTopic)
	{
		if (chan->getTopic().empty())
			client->sendMessage(":server 331 " + client->getNickname() + " " + chan->getName() + " :No topic is set\r\n");
		else
			client->sendMessage(":server 332 " + client->getNickname() + " " + chan->getName() + " :" + chan->getTopic() + "\r\n");
		return;
	}

	if (chan->isRestrictedTopic() && !chan->isOperator(client))
	{
		client->sendMessage(":server 482 " + client->getNickname() + " " + chan->getName() + " :You're not channel operator\r\n");
		return;
	}

	chan->setTopic(topic);
	chan->broadcastMessage(prefix(client) + " TOPIC " + chan->getName() + " :" + topic + "\r\n", NULL);
}

void Server::handleMode(Client* oper, const std::string& channelName, char mode, bool enable, const std::string& param)
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
			oper->sendMessage(":server 472 " + oper->getNickname() + std::string(1, mode) + " :Unknown mode\r\n");
			return;
	}

	modeMsg += "\r\n";
	chan->broadcastMessage(modeMsg, NULL);
}

// ── Nuevos handlers ───────────────────────────────────────────────────────────

void Server::handlePart(Client* client, const std::string& channelName, const std::string& reason)
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

	bool wasOperator = chan->isOperator(client);

	// Notificar a todos (incluyendo al que sale) antes de eliminarlo
	std::string msg = prefix(client) + " PART " + chan->getName();
	if (!reason.empty())
		msg += " :" + reason;
	msg += "\r\n";
	chan->broadcastMessage(msg, NULL);

	chan->removeUser(client);
	client->removeChannel(chan);

	// Si era el único operador, promover al siguiente
	if (wasOperator && chan->getCurrentUsers() > 0 && !chan->hasOperator())
		promoteNextOperator(chan);

	if (chan->getCurrentUsers() == 0)
		removeChannel(chan);
}

void Server::handleQuit(Client* client, const std::string& reason)
{
	std::string msg = prefix(client) + " QUIT :" + reason + "\r\n";

	// Notificar a todos los canales donde esté el cliente y salir de ellos
	// Trabajamos con una copia porque removeUser modifica la lista interna
	std::vector<Channel*> clientChans = client->getChannels();
	for (size_t i = 0; i < clientChans.size(); ++i)
	{
		Channel* chan = clientChans[i];
		bool wasOperator = chan->isOperator(client);

		chan->broadcastMessage(msg, client); // los demás ven el QUIT
		chan->removeUser(client);

		if (wasOperator && chan->getCurrentUsers() > 0 && !chan->hasOperator())
			promoteNextOperator(chan);

		if (chan->getCurrentUsers() == 0)
			removeChannel(chan);
	}

	// Confirmar al cliente que se va (algunos clientes lo esperan)
	client->sendMessage("ERROR :Closing link: " + reason + "\r\n");

	// La desconexión real del fd la gestiona el bucle principal del servidor;
	// aquí marcamos al cliente para que sea eliminado en el siguiente ciclo.
	client->setDisconnected(true);
}