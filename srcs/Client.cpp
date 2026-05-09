#include <algorithm>

#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"

Client::Client(const std::string nickname, const std::string& username, const int socket)
    : _nickname(nickname), _username(username), _socket(socket), _registered(false), _disconnected(false) {}

Client::~Client() {}

int Client::getClientSocket() const
{
    return _socket;
}

// ------------- getters

const std::string& Client::getNickname() const
{
    return _nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

const std::string&	 Client::getRealname() const
{
    return (_realname);
}

const std::string& Client::getPassword() const
{
	return (_password);
}

// ------------- setters

void    Client::setNickname(const std::string& nick)
{
    _nickname = nick;
}

void    Client::setUsername(const std::string& user)
{
    _username = user;
}

void    Client::setRealname(const std::string& real)
{
    _realname = real;
}

void   Client::setPassword(const std::string& pass)
{
	_password = pass;
}

//------------- registro

void    Client::setRegistered(bool val) 
{
	_registered = val;
}

bool    Client::isRegistered() const 
{
	if (_nickname == "" || _username == "" || !_registered)
		return false;
    return true;
}

//------------- canales

void Client::addChannel(Channel* channel)
{
	if (std::find(_channels.begin(), _channels.end(), channel) == _channels.end())
		_channels.push_back(channel);
}

void Client::removeChannel(Channel* channel)
{
	std::vector<Channel*>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
	if (it != _channels.end())
		_channels.erase(it);
}
const std::vector<Channel*>& Client::getChannels() const
{
	return _channels;
}

//------------- buffers de recepción

void Client::appendToRecvBuffer(const std::string& data)
{
	_recvBuffer += data;
}
std::string Client::getRecvBuffer() const
{
	return _recvBuffer;
}
void Client::clearRecvBuffer()
{
	_recvBuffer.clear();
}

//buffer de envío
void Client::sendMessage(const std::string& message)
{
	_sendBuffer += message;
}

void Client::flushSendBuffer()
{
	if (_sendBuffer.empty())
		return;
	ssize_t sent = send(_socket, _sendBuffer.c_str(), _sendBuffer.size(), 0);
	if (sent > 0)
		_sendBuffer.erase(0, sent);
}

bool Client::hasPendingMessages() const
{
	return !_sendBuffer.empty();
}

void Client::appendSendBuffer(const std::string& msg)
{
    _sendBuffer += msg;
}
std::string& Client::getSendBuffer()
{
	return _sendBuffer;
}

bool Client::isDisconnected() const
{
    return _disconnected;
}

void Client::setDisconnected(bool val)
{
    _disconnected = val;
}

bool	Client::get_register() const
{
	return (_registered);
}