#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>

/**
 * @brief handler for any msg input recv() from hexchat.
 * PRIVMSG -> any input to wich user can send an auto-reply msg
 * NOTICE -> any input to wich user can't send an auto-reply msg
 */
void Server::CommandPrivMsg(std::istringstream &iss, std::string &token, size_t selfIdx, int clientFd)
{
	std::string target;
	iss >> target;
	std::string message;
	std::getline(iss, message);

	if (!message.empty() && message[0] == ' ')
		message.erase(0, 1);
	if (message.empty() || message[0] != ':')
	{
		std::string msg = numRep(461, _clients[selfIdx].getName());
		send(clientFd, msg.c_str(), msg.size(), 0);
		return;
	}
	else if (!message.empty() && message[0] == ':')
		message.erase(0, 1);

	if (message.empty())
	{
		std::string msg = numRep(412, _clients[selfIdx].getName());
		send(clientFd, msg.c_str(), msg.size(), 0);
		return;
	}
	std::string nick = _clients[selfIdx].getName();
	if (nick.empty())
		nick = "client";

	if (!target.empty() && target[0] == '#')
	{
		std::map<std::string, Channel>::iterator it;
		printComparativeChannel(target, it, _channels);
		if (it == _channels.end() || !it->second.hasMember(clientFd))
		{
			std::cout << "ROUTE missing channel target=[" << target << "] fd=" << clientFd << std::endl;
			std::string msg = numRepChannel(404, _clients[selfIdx].getName(), target, "");
			send(clientFd, msg.c_str(), msg.size(), 0);
			return;
		}
		_clients[selfIdx].setActiveChannel(target);
		std::ostringstream prefixMsg;
		prefixMsg << ":" << nick << "!" << _clients[selfIdx].getUsername() << "@localhost " << token << " " << target << " :" << message << "\r\n";
		std::string formatted = prefixMsg.str();
		it->second.broadcastExcept(clientFd, formatted);
		if (it->second.hasMember(-1))
			it->second.botReply(nick);
		return;
	}

	for (size_t k = 0; k < _clients.size(); ++k)
	{
		std::vector<Client>::iterator it;
		printComparativeClient(target, it, _clients);

		if (_clients[k].getName() == target)
		{
			std::ostringstream prefixMsg;
			prefixMsg << ":" << nick << "!" << _clients[selfIdx].getUsername() << "@localhost " << token << " " << target << " :" << message << "\r\n";
			std::string formatted = prefixMsg.str();
			send(_clients[k].getFD(), formatted.c_str(), formatted.size(), 0);
			return;
		}
	}
	std::string msg = numRepChannel(401, nick, target, "");
	send(clientFd, msg.c_str(), msg.size(), 0);
	return;
}
