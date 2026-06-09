/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimbow <jimbow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:45:06 by jimbow            #+#    #+#             */
/*   Updated: 2026/06/09 15:06:22 by jimbow           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include <sys/socket.h>

Channel::Channel(const std::string& name) : _name(name)
{
	
}

const std::string& Channel::getName() const
{
	return _name;
}

// MEMBERS

void Channel::addMember(int fd)
{
	_members.insert(fd);
}

void Channel::removeMember(int fd)
{
	_members.erase(fd);
}

bool Channel::hasMember(int fd) const
{
	return _members.find(fd) != _members.end();
}

size_t	Channel::memberCount() const
{
	return _members.size();
}

// INVITED
void Channel::addInvite(int fd)
{
	_invited.insert(fd);
}

void Channel::removeInvite(int fd)
{
	_invited.erase(fd);
}

bool Channel::isInvited(int fd) const
{
	return _invited.find(fd) != _invited.end();
}

// OPERATOR

void Channel::addOperator(int fd)
{
	_operators.insert(fd);
}

void Channel::removeOperator(int fd)
{
	_operators.erase(fd);
}

bool Channel::isOperator(int fd) const
{
	return _operators.find(fd) != _operators.end();
}

void Channel::broadcast(const std::string& msg)
{
	for (std::set<int>::iterator it = _members.begin(); it != _members.end(); ++it)
		send(*it, msg.c_str(), msg.size(), 0);
}
