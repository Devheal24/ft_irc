/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:39:59 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/10 12:36:00 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief <unistd.h> : fonctions POSIX (close, read, write, etc.)
 */
#include "../includes/Client.hpp"
#include <unistd.h>

Client::Client(): _name(""), _fd(-1), _joinedChannels(), _activeChannel("")
{
    return;
}

Client::Client(std::string name, int fd): _name(name), _fd(fd), _joinedChannels(), _activeChannel("")
{
	return;
}

Client::~Client()
{
	/*if (_fd)
		close(_fd);*/
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
        this->_joinedChannels = base._joinedChannels;
        this->_activeChannel = base._activeChannel;
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

void Client::addClient(std::string name, int fd)
{
	_name = name;
	_fd = fd;
    return;
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
