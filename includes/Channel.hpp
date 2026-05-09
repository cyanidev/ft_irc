/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbolivar <sbolivar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:13 by pollo             #+#    #+#             */
/*   Updated: 2026/05/09 22:56:31 by sbolivar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "Client.hpp"

class Channel {
private:
std::string _name;
std::string _topic;
std::string _channelKey; // contraseña real del canal (+k)

std::vector<Client*> _users;
std::set<Client*> _operators;
std::set<Client*> _bannedUsers;
std::set<Client*> _invitedUsers; // usuarios con invitación (+i)

bool _inviteOnly; // modo +i
bool _restrictedTopic; // modo +t (solo ops pueden cambiar topic)
bool _keyRequired; // modo +k
int _maxUsers; // modo +l (0 = sin límite)

public:
Channel(const std::string& name, const std::string& topic, Client* creator);
~Channel();

// Getters básicos
const std::string& getName() const;
const std::string& getTopic() const;
const std::string& getKey() const;
const std::vector<Client*>& getUsers() const;
int getMaxUsers() const;
int getCurrentUsers() const;

// Setters básicos
void setTopic(const std::string& topic);
void setKey(const std::string& key);
void setMaxUsers(int max);

// Modos del canal
bool isInviteOnly() const;
bool isRestrictedTopic() const;
bool isKeyRequired() const;
void setInviteOnly(bool val);
void setRestrictedTopic(bool val);
void setKeyRequired(bool val);

// Comprobaciones de usuario
bool isUser(Client* user) const;
bool isOperator(Client* user) const;
bool isBanned(Client* user) const;
bool isInvited(Client* user) const;
bool hasOperator() const;

// Gestión de usuarios
void addUser(Client* user);
void removeUser(Client* user);
void addOperator(Client* user);
void removeOperator(Client* user);
void banUser(Client* user);
void addInvited(Client* user);
void removeInvited(Client* user);

// Broadcast
void broadcastMessage(const std::string& message, Client* exclude = NULL);
};

#endif