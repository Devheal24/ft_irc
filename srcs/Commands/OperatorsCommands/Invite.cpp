#include "../../../includes/Server.hpp"
#include <sstream>
#include <iostream>

void Server::CommandInvite(std::istringstream &iss, int clientFd)
{
	std::string target;
	std::string channel;

	iss >> target;
	iss >> channel;
	invite(clientFd, target, channel);
}

void Server::invite(int clientFd, std::string& targetNick, std::string& channelName)
{
	std::map<std::string, Channel>::iterator it;
	Client* client = getClientByFd(clientFd);
	std::string clientName = client->getName();

	//verify if channel exist
	printComparativeChannel(channelName, it, _channels);
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
	std::vector<Client>::iterator ite;
	printComparativeClient(targetNick, ite, _clients);
	if (ite == _clients.end())
	{
		std::string msg = numRepChannel(401, clientName, targetNick, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	// Verify if target is already in channel
	int targetFd = getClientFdByName(targetNick);
	if (ch.hasMember(targetFd))
	{
		std::string msg = numRepChannel(443, clientName, targetNick, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	// If target is Bot
	if (targetFd == -1)
	{
		ch.addMember(targetFd);
		std::cout << "Bot joined" << channelName << std::endl;
		sendNames(*client, ch, channelName);
		return ;
	}

	ch.addInvite(targetFd);
	std::string msg = numRepChannel(341, clientName, targetNick, channelName);
	send(clientFd, msg.c_str(), msg.size(), 0);
	msg = ":" + getClientPrefix(clientFd) + " INVITE " + targetNick + " :" + channelName + "\r\n";
	send(targetFd, msg.c_str(), msg.size(), 0);
}
