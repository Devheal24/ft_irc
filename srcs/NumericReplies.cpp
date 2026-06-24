/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   NumericReplies.cpp								 :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: mgarnier <mgarnier@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2026/06/17 17:18:38 by jodone			#+#	#+#			 */
/*   Updated: 2026/06/19 10:08:19 by mgarnier		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <sys/socket.h>
#include <sstream>
#include <iostream>
#include <string>
#include <ctime>

static std::string getCurrentDate()
{
	std::time_t now = std::time(NULL);
	std::tm *tm = std::localtime(&now);

	char buffer[128];
	std::strftime(buffer, sizeof(buffer), "%b %d %Y at %H:%M:%S", tm);
	return std::string(buffer);
}

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
	case 002:
		msg = msg + "Your host is yourmasterdomain, running version ft_irc-1.42\r\n";
		break;
	case 003:
	{
		std::string date = getCurrentDate();
		msg = msg + "This server was created " + date + " UTC+01:00\r\n";
		break;
	}
	case 004:
		msg = ":server 004 " + nick + " yourmasterdomain ft_irc-1.42 iotkl\r\n";
		break;
	case 412:
		msg = msg + "No text to send\r\n";
		break;
	case 433:
		msg = msg + nick + " is already in use\r\n";
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
	case 324:
		msg = msg + addon + "\r\n";
		break;
	case 331:
		msg = msg + " :No topic is set\r\n";
		break;
	case 332:
		msg = msg + ":" + addon + "\r\n";
		break;
	case 341:
		msg = msg + " " + addon + "\r\n";
		break;
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
	case 404:
		msg = msg + " :You are not in the channel\r\n";
		break;
	case 417:
		msg = msg + " :Input line was too long\r\n";
		break;
	case 433:
		msg = msg + " :Nickname is already in use\r\n";
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
		msg = msg + " :is unknown mode char " + channel + "\r\n";
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
