/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:41:32 by jimbow            #+#    #+#             */
/*   Updated: 2026/06/10 11:56:14 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <string>
#include <set>

class Channel
{
private:
	std::string		_name;
	std::set<int>	_members;
	std::set<int>	_operators;
	std::set<int>	_invited;

	std::string		_topic;
	bool			_topicRestricted; // +t mode

	// bool			_inviteOnly;
	// bool			_hasKey;
	// std::string		_key;

	// bool			_hasUserLimit;
	// size_t			_userLimit;

public:
	Channel(const std::string& name);

	const std::string& getName() const;

	//member
	void addMember(int fd);
	void removeMember(int fd);
	bool hasMember(int fd) const;
	size_t	memberCount() const;

	//invited
	void addInvite(int fd);
	void removeInvite(int fd);
	bool isInvited(int fd) const;

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

	//member function
	void broadcast(const std::string& msg);
};

#endif