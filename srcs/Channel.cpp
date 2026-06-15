/**
 * @brief <sys/socket.h> : definitions POSIX socket (send, recv, etc.)
 */
#include "../includes/Channel.hpp"
#include <sys/socket.h>
#include <iostream>

Channel::Channel(const std::string& name) : _name(name), _topicRestricted(false), _inviteOnly(false)
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

void Channel::printMembers() const
{
	for (std::set<int>::const_iterator it = _members.begin(); it != _members.end(); ++it)
		std::cout << *it << std::endl;
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

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

void Channel::setInviteOnly(bool value)
{
	_inviteOnly = value;
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
