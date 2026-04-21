#include <iostream>
#include <vector>
#include <poll.h>

#include "../includes/Client.hpp"

Client::Client(const std::string nickname, const std::string& username, const int socket)
    : _nickname(nickname), _username(username), _socket(socket), registered(false) {}

Client::~Client() {}

int Client::getClientSocket()
{
    return _socket;
}

const std::string Client::getNickname() const
{
    return _nickname;
}

const std::string Client::getUsername() const
{
    return _username;
}

void    Client::setNickname(std::string nick)
{
    _nickname = nick;
}

void    Client::setUsername(std::string user)
{
    _username = user;
}

bool    Client::registeredStatus() const
{
    return (registered);
}

void    Client::isRegistered()
{
    registered = true;
}

void    Client::setRealname(std::string real)
{
    _realname = real;
}

const std::string	 Client::getRealname() const
{
    return (_realname);
}