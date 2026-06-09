/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarnier <mgarnier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:39:59 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/09 23:31:09 by mgarnier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <unistd.h>

Client::Client(): _name(NULL), _fd(-1)
{
    return;
}

Client::Client(std::string name, int fd): _name(name), _fd(fd)
{
	return;
}

Client::~Client()
{
	if (_fd)
		close(_fd);
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
