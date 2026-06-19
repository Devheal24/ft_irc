#include "../../../includes/Server.hpp"
#include <sstream>

void Server::CommandInvite(std::istringstream &iss, int clientFd)
{
	std::string target;
	std::string channel;

	iss >> target;
	iss >> channel;
	invite(clientFd, target, channel);
}

void Server::invite(int clientFd, const std::string& targetNick, const std::string& channelName)
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

	//verify if client is operator
	if (!ch.isOperator(clientFd))
	{
		std::string msg = numRepChannel(482, clientName, channelName, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	// Verify if target exist
	int targetFd = getClientFdByName(targetNick);
	if (targetFd == -1)
	{
		std::string msg = numRepChannel(441, clientName, targetNick, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	// Verify if target is already in channel
	if (ch.hasMember(targetFd))
	{
		std::string msg = numRepChannel(443, clientName, targetNick, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	ch.addInvite(targetFd);
	std::string msg = numRepChannel(341, clientName, targetNick, channelName);
	send(clientFd, msg.c_str(), msg.size(), 0);
	msg = ":" + getClientPrefix(clientFd) + " INVITE " + targetNick + " :" + channelName + "\r\n";
	send(targetFd, msg.c_str(), msg.size(), 0);
}
