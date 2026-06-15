/**
 * @brief <unistd.h> : functions POSIX (close, read, write, ...)
 */
#include "../includes/Client.hpp"
#include <unistd.h>

Client::Client(): _name(""), _fd(-1), _ip(0), _joinedChannels(), _activeChannel(""), _username(""), _realname(""), _pass(""), _hasPass(false), _hasNick(false), _hasUser(false), _registered(false)
{
    return;
}

Client::Client(std::string name, int fd, std::string ip): _name(name), _fd(fd), _ip(ip), _joinedChannels(), _activeChannel(""), _username(""), _realname(""), _pass(""), _hasPass(false), _hasNick(false), _hasUser(false), _registered(false)
{
	return;
}

Client::~Client()
{
    return;
}

Client::Client(const Client &base)
{
    *this = base;
    return;
}

Client &Client::operator=(const Client &base)
{
    if (this != &base)
    {
		this->_name = base._name;
        this->_fd = base._fd;
        this->_ip = base._ip;
        this->_joinedChannels = base._joinedChannels;
        this->_activeChannel = base._activeChannel;
        this->_username = base._username;
        this->_realname = base._realname;
        this->_pass = base._pass;
        this->_hasPass = base._hasPass;
        this->_hasNick = base._hasNick;
        this->_hasUser = base._hasUser;
        this->_registered = base._registered;
    }
    return (*this);
}

std::string Client::getName() const
{
	return (_name);
}

int Client::getFD() const
{
	return (_fd);
}

std::string Client::getIP() const
{
    return (_ip);
}

std::string Client::getUsername() const
{
    return (_username);
}

void Client::setFD(int fd)
{
    _fd = fd;
    return;
}

void Client::addClient(std::string name, int fd)
{
	_name = name;
	_fd = fd;
    return;
}

void Client::setNick(const std::string& nick)
{
    _name = nick;
    _hasNick = true;
    if (_hasUser && _hasNick)
        _registered = true;
}

void Client::setUser(const std::string& user, const std::string& real)
{
    _username = user;
    _realname = real;
    _hasUser = true;
    if (_hasUser && _hasNick)
        _registered = true;
}

void Client::setPass(const std::string& pass)
{
    _pass = pass;
    _hasPass = true;
}

bool Client::isRegistered() const
{
    return _registered;
}

std::string Client::getPass() const
{
    return _pass;
}

bool Client::hasPass() const
{
    return _hasPass;
}

void Client::joinChannel(const std::string& channelName)
{
    _joinedChannels.insert(channelName);
    _activeChannel = channelName;
}

void Client::partChannel(const std::string& channelName)
{
    _joinedChannels.erase(channelName);
    if (_activeChannel == channelName)
        _activeChannel = "";
}

void Client::setActiveChannel(const std::string& channelName)
{
    if (_joinedChannels.find(channelName) != _joinedChannels.end())
        _activeChannel = channelName;
}

std::string Client::getActiveChannel() const
{
    return _activeChannel;
}

bool Client::isInChannel(const std::string& channelName) const
{
    return _joinedChannels.find(channelName) != _joinedChannels.end();
}

const std::set<std::string>& Client::getJoinedChannels() const
{
    return _joinedChannels;
}
