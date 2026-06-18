/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jodone <jodone@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 17:18:38 by jodone            #+#    #+#             */
/*   Updated: 2026/06/18 16:35:27 by jodone           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
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
		break;
	case 433:
		msg = msg + "Nickname is already in use\r\n";
		break;
	case 461:
		msg = msg + "Not enough parameters\r\n";
		break;
	case 464:
		msg = msg + "Password incorrect\r\n";
		break;
	case 901:
		msg = msg + "You are now logged out\r\n";
		break;
	default:
		break;
	}
	return msg;
}

std::string numRepChannel(int code, const std::string& nick, const std::string& channel, const std::string& addon)
{
	std::ostringstream oss;
	oss << ":server " << code << " " << nick << " " << channel;
	std::string msg = oss.str();
	switch (code)
	{
	case 331:
		msg = msg + " :No topic is set\r\n";
		break;
	case 332:
		msg = msg + ":" + addon + "\r\n";
		break;
	case 341:
		msg = msg + " " + addon + "\r\n";
	case 353:
		msg = ":server 353 " + nick + " = " + channel + " :" + addon + "\r\n";
		break;
	case 366:
		msg = msg + " :End of /NAMES list\r\n";
		break;
	case 401:
		msg = msg + " :No such nick/channel\r\n";
		break;
	case 403:
		msg = msg + " :No such channel\r\n";
		break;
	case 441:
		msg = msg + " :No such nick on that channel\r\n";
		break;
	case 442:
		msg = msg + " :You're not on that channel\r\n";
		break;
	case 443:
		msg = msg + " :Is already on channel\r\n";
		break;
	case 471:
		msg = msg + " :Cannot join, channel is full (+l)\r\n";
		break;
	case 472:
		msg = msg + " :is unknown mode char" + channel + "\r\n";
		break;
	case 473:
		msg = msg + " :Cannot join, channel is in invite only (+i)\r\n";
		break;
	case 475:
		msg = msg + " :Cannot join channel, bad password (+k)\r\n";
		break;
	case 482:
		msg = msg + " :You're not channel operator\r\n";
		break;
	default:
		break;
	}
	return msg;
}
