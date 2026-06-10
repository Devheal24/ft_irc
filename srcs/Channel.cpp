/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:45:06 by jimbow            #+#    #+#             */
/*   Updated: 2026/06/10 12:36:04 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @brief <sys/socket.h> : définitions POSIX socket (send, recv, etc.)
 */
#include "Channel.hpp"
#include <sys/socket.h>

Channel::Channel(const std::string& name) : _name(name), _topicRestricted(false)
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

// TOPIC

const std::string& Channel::getTopic() const
{
	return _topic;
}

void Channel::setTopic(const std::string& topic)
{
	_topic = topic;
}

bool Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

void Channel::setTopicRestricted(bool restricted)
{
	_topicRestricted = restricted;
}

void Channel::broadcast(const std::string& msg)
{
	for (std::set<int>::iterator it = _members.begin(); it != _members.end(); ++it)
		send(*it, msg.c_str(), msg.size(), 0);
}

/**
 * @brief avoid sending to exclude fd
 */
void Channel::broadcastExcept(int excludeFd, const std::string& msg)
{
	for (std::set<int>::iterator it = _members.begin(); it != _members.end(); ++it)
		if (*it != excludeFd)
			send(*it, msg.c_str(), msg.size(), 0);
}
