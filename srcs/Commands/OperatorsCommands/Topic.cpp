#include "../../../includes/Server.hpp"
#include <sstream>

void Server::CommandTopic(std::istringstream &iss, int clientFd)
{
	std::string channel;
	std::string NewTopic;

	iss >> channel;
	iss >> NewTopic;
	topic(clientFd, channel, NewTopic);
}

void Server::topic(int clientFd, const std::string& channelName, std::string& newTopic)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	Client* client = getClientByFd(clientFd);
	std::string clientName = client->getName();

	//verify if channel exist
	if (it == _channels.end())
	{
		std::string msg = numRepChannel(403, clientName, channelName, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	Channel& ch = it->second;

	//verify if client is in channel
	if (!ch.hasMember(clientFd))
	{
		std::string msg = numRepChannel(442, clientName, channelName, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	//if newTopic empty, display actual topic if there is one
	if (newTopic.empty())
	{
		if (ch.getTopic().empty())
		{
			std::string msg = numRepChannel(331, clientName, channelName, "");
			send(clientFd, msg.c_str(), msg.size(), 0);
		}
		else
		{
			std::string msg = numRepChannel(332, clientName, channelName, ch.getTopic());
			send(clientFd, msg.c_str(), msg.size(), 0);
		}
		return ;
	}

	//verify if client is operator and topic restricted
	if (ch.isTopicRestricted() && !ch.isOperator(clientFd))
	{
		std::string msg = numRepChannel(482, clientName, channelName, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	if (newTopic[0] == ' ')
		newTopic.erase(0, 1);
	if (newTopic[0] == ':')
		newTopic.erase(0, 1);
	ch.setTopic(newTopic);
	std::string msg = ":" + getClientPrefix(clientFd) + " TOPIC " + channelName + " :" + newTopic + "\r\n";
	ch.broadcast(msg);
}
