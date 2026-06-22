#include "../../../includes/Server.hpp"
#include <sstream>
#include <cstdlib>

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

	bool sign = (mode[0] == '+');
	if (mode.size() > 2)
	{
		std::string msg = numRepChannel(472, clientName, mode, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}
	for (size_t i = 1; i < mode.size(); i++)
	{
		switch (mode[i])
		{
		case 'i':
			ch.setInviteOnly(sign);
			ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");
			break;
		case 't':
			ch.setTopicRestricted(sign);
			ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");
			break;
		case 'k':
		{
			std::string key;
			iss >> key;
			if (key.empty())
			{
				std::string msg = numRep(461, clientName);
				send(clientFd, msg.c_str(), msg.size(), 0);
				return ;
			}
			if (sign == true)
			{
				if (ch.getKey().empty())
				{
					ch.setKey(key);
					ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + " " + key + "\r\n");	
					break;
				}
			}
			else
			{
				if (ch.getKey() == key)
				{
					ch.removeKey();
					ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");
					break;
				}
			}
			std::string msg = ":server NOTICE " + channel + " :" + channel + " :Key is already set\r\n";
			send(clientFd, msg.c_str(), msg.size(), 0);
			break;
		}
		case 'o':
		{
			std::string targetName;
			iss >> targetName;

			 if (targetName.empty())
			{
				std::string msg = numRep(461, clientName);
				send(clientFd, msg.c_str(), msg.size(), 0);
				return ;
			}
			int targetFd = getClientFdByName(targetName);
			if (targetFd == -1)
			{
				std::string msg = numRepChannel(401, clientName, targetName, "");
				send(clientFd, msg.c_str(), msg.size(), 0);
				return ;
			}
			if (sign == true)
				ch.addOperator(targetFd);
			else
				ch.removeOperator(targetFd);
			ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + " " + targetName + "\r\n");
			break;
		}
		case 'l':
		{
			if (sign == true)
			{
				std::string param;
				iss >> param;

				if (param.empty())
				{
					std::string msg = numRep(461, clientName);
					send(clientFd, msg.c_str(), msg.size(), 0);
					return ;
				}
				char *end;
				long limit = std::strtol(param.c_str(), &end, 10);
				
				if (*end != '\0' || limit <= 0 || limit > 50)
					return ;
				ch.setLimit(static_cast<size_t>(limit));
				std::ostringstream oss;
				oss << ":" << getClientPrefix(clientFd) << " MODE " << channel << " " << mode << " " << limit << "\r\n";
				std::string s = oss.str();
				ch.broadcast(s);
			}
			else
			{
				if (ch.haslimit())
				{
					ch.removeLimit();
					ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");	
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
