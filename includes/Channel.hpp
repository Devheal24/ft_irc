/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:41:32 by jimbow            #+#    #+#             */
/*   Updated: 2026/06/09 14:13:31 by jhubier          ###   ########.fr       */
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

	bool			_inviteOnly;
	bool			_topicRestricted;
	bool			_hasKey;
	std::string		_key;

	bool			_hasUserLimit;
	size_t			_userLimit;

public:
	Channel(const std::string& name);

	const std::string& getName() const;

	void addMember(int fd);
	void removeMember(int fd);
	bool hasMember(int fd) const;
	size_t	memberCount() const;

	bool isOperator(int fd) const;
};

#endif