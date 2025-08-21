/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:51:45 by pollo             #+#    #+#             */
/*   Updated: 2025/08/21 15:24:36 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel(const std::string& name, const std::string& topic, client* creator) {
    _name = "#" + name;
    _topic = topic;
    _users.push_back(creator);
    _operators.insert(creator);
    _inviteOnlyChannel = false;
    _restrictedTopic = false;
    _channelKeyRequired = false;
    _channelOperatorPrivilege = false;
    _maxUsers = 0; 

    std::cout << "Channel " << _name << " created" << std::endl;
}

Channel::~Channel() {
    _users.clear();
    _operators.clear();
    _bannedUsers.clear();
    _name.clear();
    _topic.clear();

    std::cout << "Channel " << _name << ": deleted" << std::endl;
}

const std::string	Channel::getChannelName() {
    return _name;
}

const std::string	Channel::getChannelTopic() {
    return _topic;
}

void	Channel::setChannelTopic(const std::string& topic) {
    _topic = topic;
}

void	Channel::addUser(client* user) {
    if (std::find(_users.begin(), _users.end(), user) != _users.end()) {
        std::cout << "User " << user->getNickname() << " is already in Channel " << _name << std::endl;
        return;
    }
    if (_maxUsers != 0 && _currentUsers >= _maxUsers) {
        std::cout << "Channel " << _name << " is full. Cannot add user " << user->getNickname() << std::endl;
        return;
    }
    _users.push_back(user);
    std::cout << "User " << user->getNickname() << " added to Channel " << _name << std::endl;
}

void	Channel::addOperator(client* user) {
    if (_operators.find(user) != _operators.end()) {
        return;
    }
    _operators.insert(user);

    std::cout << "User " << user->getNickname() << " is now an operator in Channel " << _name << std::endl;
}

void	Channel::removeOperator(client* user) {
    if (_operators.find(user) == _operators.end()) {
        return;
    }
    _operators.erase(user);

    std::cout << "User " << user->getNickname() << " is no longer an operator in Channel " << _name << std::endl;
}

void	Channel::removeUser(client* user) {
    std::vector<client*>::iterator it = std::find(_users.begin(), _users.end(), user);
    if (it == _users.end()) {
        std::cout << "User " << user->getNickname() << " not found in Channel " << _name << std::endl;
        return;
    }
    _users.erase(it);
    if (_operators.find(user) != _operators.end()) {
        _operators.erase(user);
    }

    std::cout << "User " << user->getNickname() << " removed from Channel " << _name << std::endl;
}

void	Channel::banUser(client* user) {
    if (_bannedUsers.find(user) != _bannedUsers.end()) {
        return;
    }
    _bannedUsers.insert(user);
    removeUser(user);
    removeOperator(user);

    std::cout << "User " << user->getNickname() << " is banned from Channel " << _name << std::endl;
}

bool	Channel::getInviteOnlyChannel() const {
    return _inviteOnlyChannel;
}

bool	Channel::getRestrictedTopic() const {
    return _restrictedTopic;
}

bool	Channel::getChannelKeyRequired() const {
    return _ChannelKeyRequired;
}

bool	Channel::getChannelOperatorPrivilege() const {
    return _ChannelOperatorPrivilege;
}

int		Channel::getMaxUsers() const {
    return _maxUsers;
}

int		Channel::getCurrentUsers() const {
    return (int)_users.size();
}

void	Channel::setInviteOnlyChannel(bool inviteOnly) {
    _inviteOnlyChannel = inviteOnly;
}

void	Channel::setRestrictedTopic(bool restricted) {
    _restrictedTopic = restricted;
}

void	Channel::setChannelKeyRequired(bool keyRequired) {
    _ChannelKeyRequired = keyRequired;
}

void	Channel::setChannelOperatorPrivilege(bool operatorPrivilege) {
    _ChannelOperatorPrivilege = operatorPrivilege;
}

void	Channel::setMaxUsers(int maxUsers) {
    _maxUsers = maxUsers;
}