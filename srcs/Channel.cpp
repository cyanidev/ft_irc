/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Andie <Andie@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:51:45 by pollo             #+#    #+#             */
/*   Updated: 2026/05/08 13:38:07 by Andie            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include "../includes/Channel.hpp"

Channel::Channel(const std::string& name, const std::string& topic, Client* creator)
: _name("#" + name), _topic(topic), _channelKey(""),
_inviteOnly(false), _restrictedTopic(false), _keyRequired(false), _maxUsers(0)
{
_users.push_back(creator);
_operators.insert(creator);
std::cout << "Channel " << _name << " created by " << creator->getNickname() << std::endl;
}

Channel::~Channel() {
_users.clear();
_operators.clear();
_bannedUsers.clear();
_invitedUsers.clear();
}

// --- Getters -------------------------------------------------

const std::string& Channel::getName() const { return _name; }
const std::string& Channel::getTopic() const { return _topic; }
const std::string& Channel::getKey() const { return _channelKey; }
const std::vector<Client*>& Channel::getUsers() const { return _users; }
int Channel::getMaxUsers() const { return _maxUsers; }
int Channel::getCurrentUsers() const { return (int)_users.size(); }

// --- Setters -------------------------------------------------------

void Channel::setTopic(const std::string& topic) { _topic = topic; }
void Channel::setKey(const std::string& key) { _channelKey = key; }
void Channel::setMaxUsers(int max) { _maxUsers = max; }

// --- Modos ---................................................

bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isRestrictedTopic() const { return _restrictedTopic; }
bool Channel::isKeyRequired() const { return _keyRequired; }
void Channel::setInviteOnly(bool val) { _inviteOnly = val; }
void Channel::setRestrictedTopic(bool val) { _restrictedTopic = val; }
void Channel::setKeyRequired(bool val) { _keyRequired = val; }

// --- Comprobaciones ------------------------------------------

bool Channel::isUser(Client* user) const
{
	return std::find(_users.begin(), _users.end(), user) != _users.end();
}

bool Channel::isOperator(Client* user) const
{
	return _operators.find(user) != _operators.end();
}

bool Channel::isBanned(Client* user) const
{
	return _bannedUsers.find(user) != _bannedUsers.end();
}

bool Channel::isInvited(Client* user) const
{
	return _invitedUsers.find(user) != _invitedUsers.end();
}

// --- Gestión de usuarios ----------------------------------------

void Channel::addUser(Client* user)
{
	if (isUser(user))
		return;
	if (_maxUsers > 0 && getCurrentUsers() >= _maxUsers)
	{
		std::cout << "Channel " << _name << " is full" << std::endl;
		return;
	}
	_users.push_back(user);
	removeInvited(user); // ya no necesita la invitación una vez dentro
}

void Channel::removeUser(Client* user)
{
	std::vector<Client*>::iterator it = std::find(_users.begin(), _users.end(), user);
	if (it == _users.end()) return;
	_users.erase(it);
	_operators.erase(user);
}

void Channel::addOperator(Client* user)
{
	if (!isUser(user)) return; // solo puede ser op si está en el canal
	_operators.insert(user);
}

void Channel::removeOperator(Client* user)
{
	_operators.erase(user);
}

void Channel::banUser(Client* user)
{
	_bannedUsers.insert(user);
	removeUser(user);
}

void Channel::addInvited(Client* user)
{
	_invitedUsers.insert(user);
}

void Channel::removeInvited(Client* user)
{
	_invitedUsers.erase(user);
}

// --- Broadcast --------------------------------------------

void Channel::broadcastMessage(const std::string& message, Client* exclude)
{
	for (std::vector<Client*>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (*it != exclude)
		(*it)->sendMessage(message); // asumiendo que Client tiene sendMessage()
	}
}
