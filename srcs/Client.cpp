#include <iostream>
#include <vector>
#include <poll.h>

#include "../includes/Client.hpp"

Client::Client(const std::string nickname, const std::string& username, const int socket)
    : _nickname(nickname), _username(username), _socket(socket) {}

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