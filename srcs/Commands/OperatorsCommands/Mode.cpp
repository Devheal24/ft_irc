#include "../../../includes/Server.hpp"
#include <sstream>
#include <cstdlib>
#include <iostream>

void Server::CommandMode(std::istringstream &iss, int clientFd)
{
	Client* client = getClientByFd(clientFd);
	std::string clientName = client->getName();
	std::string channel;
	std::string mode;

	iss >> channel;
	iss >> mode;

	//verify if channel exist
	std::map<std::string, Channel>::iterator it = _channels.find(channel);
	if (it == _channels.end())
	{
		std::string msg = numRepChannel(403, clientName, channel, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}
	Channel& ch = it->second;

	//verify is client is operator
	if (!ch.isOperator(clientFd))
	{
		std::string msg = numRepChannel(482, clientName, channel, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	int		count[3] = {0, 0, 0};
	bool	sign;
	switch (mode[0])
	{
	case '+':
		sign = true;
		break;
	case '-':
		sign = false;
		break;
	default:
		std::string msg = numRepChannel(472, clientName, mode, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return;
	}

	for (size_t i = 1; i < mode.size(); i++)
	{
		switch (mode[i])
		{
		case 'i':
			if ((!ch.isInviteOnly() && sign) || (ch.isInviteOnly() && !sign))
			{
				ch.setInviteOnly(sign);
				ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode[0] + mode[i] + "\r\n");
			}
			break;
		case 't':
			if ((!ch.isTopicRestricted() && sign) || (ch.isTopicRestricted() && !sign))
			{
				ch.setTopicRestricted(sign);
				ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode[0] + mode[i] + "\r\n");
			}
			break;
		case 'k':
		{
			if (count[0] > 0)
				break;
			count[0]++;
			std::string key;
			iss >> key;
			if (key.empty())
			{
				std::string msg = numRep(461, clientName);
				send(clientFd, msg.c_str(), msg.size(), 0);
				break;
			}
			if (sign == true)
			{
				if (ch.getKey().empty())
				{
					ch.setKey(key);
					ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode[0] + mode[i] + " " + key + "\r\n");	
					break;
				}
			}
			else
			{
				if (ch.getKey() == key)
				{
					ch.removeKey();
					ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode[0] + mode[i] + "\r\n");
					break;
				}
			}
			std::string msg = ":server NOTICE " + channel + " :" + channel + " :Key is already set\r\n";
			send(clientFd, msg.c_str(), msg.size(), 0);
			break;
		}
		case 'o':
		{
			if (count[1] > 0)
				break;
			count[1]++;
			std::string targetName;
			iss >> targetName;

			 if (targetName.empty())
			{
				std::string msg = numRep(461, clientName);
				send(clientFd, msg.c_str(), msg.size(), 0);
				break;
			}
			int targetFd = getClientFdByName(targetName);
			if (targetFd == -1)
			{
				std::string msg = numRepChannel(401, clientName, targetName, "");
				send(clientFd, msg.c_str(), msg.size(), 0);
				break;
			}
			if (sign == true)
				ch.addOperator(targetFd);
			else
				ch.removeOperator(targetFd);
			ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode[0] + mode[i] + " " + targetName + "\r\n");
			break;
		}
		case 'l':
		{
			if (count[2] > 0)
				break;
			count[2]++;
			if (sign == true)
			{
				std::string param;
				iss >> param;

				if (param.empty())
				{
					std::string msg = numRep(461, clientName);
					send(clientFd, msg.c_str(), msg.size(), 0);
					break;
				}
				char *end;
				long limit = std::strtol(param.c_str(), &end, 10);
				
				if (*end != '\0' || limit <= 0 || limit > 50)
					break;
				ch.setLimit(static_cast<size_t>(limit));
				std::ostringstream oss;
				oss << ":" << getClientPrefix(clientFd) << " MODE " << channel << " " << mode[0] << mode[i] << " " << limit << "\r\n";
				std::string s = oss.str();
				ch.broadcast(s);
			}
			else
			{
				if (ch.haslimit())
				{
					ch.removeLimit();
					ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode[0] + mode[i] + "\r\n");	
				}
			}
			break;
		}
		default:
			std::string msg = numRepChannel(472, clientName, mode, "");
			send(clientFd, msg.c_str(), msg.size(), 0);
			break;
		}
	}
}
