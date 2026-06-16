#pragma once
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

/**
 * @include <string> : stockage des noms et opérations sur chaînes
 * @include <set> : structure pour membres/operateurs/invitations sans doublons
 */
#include <string>
#include <set>
#include <vector>

class Channel
{
private:
	std::string		_name;
	std::set<int>	_members;
	std::set<int>	_operators;

	std::set<int>	_invited;
	bool			_inviteOnly;

	std::string		_topic;
	bool			_topicRestricted;

	std::string		_key;
	bool			_hasKey;

	size_t			_userLimit;
	bool			_hasUserLimit;

	std::vector<std::string> _botMessages;
	bool			_botEnabled;

public:
	Channel(const std::string& name);

	const std::string& getName() const;

	//member
	void addMember(int fd);
	void removeMember(int fd);
	bool hasMember(int fd) const;
	size_t	memberCount() const;
	void printMembers() const;

	//invited
	void addInvite(int fd);
	void removeInvite(int fd);
	bool isInvited(int fd) const;
	bool isInviteOnly() const;
	void setInviteOnly(bool value);

    // broadcast helpers
    void broadcastExcept(int excludeFd, const std::string& msg);

	//operators
	void addOperator(int fd);
	void removeOperator(int fd);
	bool isOperator(int fd) const;

	//topic
	const std::string& getTopic() const;
	void setTopic(const std::string& topic);
	bool isTopicRestricted() const;
	void setTopicRestricted(bool restricted);

	//key
	bool hasKey() const;
	const std::string& getKey() const;
	void setKey(const std::string& key);
	void removeKey();

	//limit
	void setLimit(size_t limit);
	void removeLimit();
	bool isFull() const;

	//bot
	void setBotEnabled(bool value);
	bool isBotEnabled() const;
	void botReply(int clientFd);

	//member function
	void broadcast(const std::string& msg);

};

#endif