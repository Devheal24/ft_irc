/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jodone <jodone@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:18:38 by jodone            #+#    #+#             */
/*   Updated: 2026/06/17 17:47:18 by jodone           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/socket.h>
#include <sstream>
#include <iostream>
#include <string>

std::string numRep(int code, const std::string& nick)
{
	std::ostringstream oss;
	if (code < 10)
		oss << ":server 00" << code << " " << nick << " :";
	else
		oss << ":server " << code << " " << nick << " :";
	std::string msg = oss.str();
	switch (code)
	{
	case 001:
		msg = msg + "Welcome to the IRC server, " + nick + "\r\n";
	case 464:
		msg = msg + "Password incorrect\r\n";
		break;
	
	default:
		break;
	}
	return msg;
}

std::string numRepChannel(int code, const std::string& nick, const std::string& channel)
{
	(void)code;
	(void)nick;
	(void)channel;
	return NULL;
}

std::string numRepTarget(int code, const std::string& nick, const std::string& target)
{
	(void)code;
	(void)nick;
	(void)target;
	return NULL;
}
