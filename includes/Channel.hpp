/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimbow <jimbow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:41:32 by jimbow            #+#    #+#             */
/*   Updated: 2026/06/09 15:04:34 by jimbow           ###   ########.fr       */
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

	// bool			_inviteOnly;
	// bool			_topicRestricted;
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

	//operators
	void addOperator(int fd);
	void removeOperator(int fd);
	bool isOperator(int fd) const;

	//member function
	void broadcast(const std::string& msg);
};

#endif