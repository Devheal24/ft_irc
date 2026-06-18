/**
 * @brief <sys/socket.h> : definitions POSIX socket (send, recv, etc.)
 */
#include "../includes/Channel.hpp"
#include <sys/socket.h>
#include <iostream>
#include <cstdlib>

Channel::Channel(const std::string& name) : _name(name), _inviteOnly(false), _topicRestricted(false), _hasKey(false), _userLimit(0), _hasUserLimit(false)
{
	_botMessages.push_back("Salut à tous !");
	_botMessages.push_back("AH ! Le négociateur...");
	_botMessages.push_back("C'est pas faux.");
	_botMessages.push_back("Non je peux pas, j'ai AquaPoney.");
	_botMessages.push_back("De quoi ?");
	_botMessages.push_back("Oui.");
	_botMessages.push_back("Chut !");
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

// KEY
bool Channel::hasKey() const
{
	return _hasKey;
}

const std::string& Channel::getKey() const
{
	return _key;
}

void Channel::setKey(const std::string& key)
{
	_key = key;
	_hasKey = true;
}

void Channel::removeKey()
{
	_key.clear();
	_hasKey = false;
}

//LIMIT
void Channel::setLimit(size_t limit)
{
	_userLimit = limit;
	_hasUserLimit = true;
}

void Channel::removeLimit()
{
	_userLimit = 0;
	_hasUserLimit = false;
}

bool Channel::isFull() const
{
	return _hasUserLimit && _members.size() >= _userLimit;
}

bool Channel::haslimit() const
{
	return _hasUserLimit;
}

//BOT
void Channel::botReply(int clientFd)
{
	if (_members.size() == 0)
		return ;
	
	int r = rand() % _botMessages.size();
	std::string msg = _botMessages[r];
	std::string full = ":BOT!bot@localhost PRIVMSG " + _name + " :" + msg + "\r\n";
	broadcastExcept(clientFd, full);
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
