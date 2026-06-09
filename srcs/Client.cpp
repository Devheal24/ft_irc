/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarnier <mgarnier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:39:59 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/09 18:25:17 by mgarnier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client()
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
    }
    return (*this);
}

std::string Client::getName(int fd) const
{
	for (int i = 0; i < static_cast<int>(_fd.size()); i++)
		if (i == fd)
			return (_name[i]);
	return (NULL);
}

int Client::getFD(std::string name) const
{
	for (size_t i = 0; i < _name.size(); i++)
		if (_name[i].compare(name))
			return (_fd[i]);
	return (-1);
}

void Client::addClient(std::string name, int fd)
{
	_name.push_back(name);
	_fd.push_back(fd);
    return;
}
