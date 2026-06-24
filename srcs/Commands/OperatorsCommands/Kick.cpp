#include "../../../includes/Server.hpp"
#include <sstream>

void Server::CommandKick(std::istringstream &iss, int clientFd)
{
	std::string channel;
	std::string target;
	std::string reason;

	iss >> channel;
	iss >> target;
	std::getline(iss, reason);
	if (!reason.empty() && reason[0] == ' ')
		reason.erase(0, 1);
	if (!reason.empty() && reason[0] == ':')
		reason.erase(0, 1);
	kick(clientFd, channel, target, reason);
}

void Server::kick(int clientFd, std::string& channelName, std::string& targetName, const std::string& reason)
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

	// Verify if target exist and is in channel
	std::map<int, Client> _clients;

	int targetFd = getClientFdByName(targetName);
	if (!ch.hasMember(targetFd))
	{
		std::string msg = numRepChannel(441, clientName, targetName, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	ch.printMembers();
	std::string msg = ":" + getClientPrefix(clientFd) + " KICK " + channelName + " " + targetName + " :" + reason + "\r\n";
	ch.broadcast(msg);

	ch.removeMember(targetFd);
	ch.removeOperator(targetFd);
}
