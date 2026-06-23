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
	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);

	std::string nick = _clients[selfIdx].getName();
	if (nick.empty())
		nick = "client";

	if (!target.empty() && (target[0] == '#' || target[0] == '&'))
	{
		std::map<std::string, Channel>::iterator it = _channels.find(target);
		if (it == _channels.end())
		{
			std::cout << "ROUTE missing channel target=[" << target << "] fd=" << clientFd << std::endl;
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

	bool delivered = false;
	for (size_t k = 0; k < _clients.size(); ++k)
	{
		if (_clients[k].getFD() == clientFd)
			continue;
		if (_clients[k].getName() == target)
		{
			std::ostringstream prefixMsg;
			prefixMsg << ":" << nick << "!" << _clients[selfIdx].getUsername() << "@localhost " << token << " " << target << " :" << message << "\r\n";
			std::string formatted = prefixMsg.str();
			send(_clients[k].getFD(), formatted.c_str(), formatted.size(), 0);
			delivered = true;
			break;
		}
	}
	if (!delivered)
	{
		std::string msg = numRepChannel(401, nick, target, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
	}
	return;
}
