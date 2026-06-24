#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>

/**
 * @brief handler for JOIN cmd recv() from hexchat.
 * JOIN -> let you join or set as active a new channel
 */
std::vector<std::string> splitComma(const std::string &name)
{
	std::vector<std::string> tmp;
	std::stringstream iss(name);
	std::string buf;
	while (getline(iss, buf, ','))
	{
		tmp.push_back(buf);
	}
	for (size_t i = 0; i < tmp.size(); ++i)
		std::cout << "tmp[i]= " << tmp[i] << std::endl;
	return (tmp);
}

void Server::CommandJoin(std::istringstream &iss, int clientFd)
{
	Client* client = getClientByFd(clientFd);
	std::string chan;
	std::string key;
	iss >> chan;
	iss >> key;

	if (!chan.empty() && chan[0] == ':')
		chan = chan.substr(1);

	while (!chan.empty() &&
			(chan[chan.size() - 1] == '\r' || chan[chan.size() - 1] == '\n')) 
		chan.resize(chan.size() - 1);

	std::vector<std::string> tmp = splitComma(chan);

	for (size_t i = 0; i < tmp.size(); ++i)
	{
		chan = tmp[i];
		if (!chan.empty() && chan[0] == '#')
		{
			if (chan.length() >= CHNL_MAXL || chan.length() < CHNL_MINL)
			{
				std::cout << "invalid length of channel name (>20)" << chan << std::endl;
				std::string msg =":server NOTICE :invalid length of channel name (20> x <1)\r\n";
				send(clientFd, msg.c_str(), msg.length(), 0);
				continue;
			}
			for (size_t i = 1; i < chan.length(); i++)
			{
				if (!isdigit(chan[i]) && !isalnum(chan[i]) && chan[i] != '_' && chan[i] != '-')
				{
					std::cout << "invalid character in channel name" << chan << std::endl;
					continue;
				}
			}
			joinChannel(clientFd, chan, key);
		}
		else
		{
			std::string msg = numRepChannel(403, client->getName(), chan, "");
			send(clientFd, msg.c_str(), msg.size(), 0);
		}
	}
	return;
}

void Server::joinChannel(int clientFd, const std::string& name, const std::string& key)
{
	std::map<std::string, Channel>::iterator it;
	Client* client = getClientByFd(clientFd);
	std::string clientName = client->getName();

	std::cout << "attempting to join channel : " << name << std::endl;
	it = _channels.find(name);

	if (it == _channels.end())
	{
		_channels.insert(std::make_pair(name, Channel(name)));
		it = _channels.find(name);
		std::cout << "Channel created: " << name << std::endl;
	}
	
	// if client already member, just set active channel
	if (it->second.hasMember(clientFd))
	{
		size_t j = 0;
		while (j < _clients.size() && _clients[j].getFD() != clientFd)
			++j;
		_clients[j].setActiveChannel(name);
		std::ostringstream oss;
		oss << ":server NOTICE " << clientFd << " :Now active in " << name << "\r\n";
		std::string msg = oss.str();
		send(clientFd, msg.c_str(), msg.size(), 0);
		std::cout << "Client " << clientFd << " set active " << name << std::endl;
		return;
	}

	// Check if channel is InviteOnly
	if (it->second.isInviteOnly() == true)
	{
		if (!it->second.isInvited(clientFd))
		{
			std::string msg = numRepChannel(473, clientName, name, "");
			send(clientFd, msg.c_str(), msg.size(), 0);
			return ;
		}
	}
	it->second.removeInvite(clientFd);

	// Check if channel has key and if key is matching
	if (it->second.hasKey())
	{
		if (key != it->second.getKey())
		{
			std::string msg = numRepChannel(475, clientName, name, "");
			send(clientFd, msg.c_str(), msg.size(), 0);
			return ;
		}
	}

	// Check if channel is full with limit enable
	if (it->second.isFull())
	{
		std::string msg = numRepChannel(471, clientName, name, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return ;
	}

	// firstMember is def operator
	bool firstMember = (it->second.memberCount() == 0);
	it->second.addMember(clientFd);
	if (firstMember)
	{
		it->second.addOperator(clientFd);
		std::cout << "Client " << clientFd << " is operator" << std::endl;
	}
	std::cout << "Client " << clientFd << " joined " << name << std::endl;

	size_t j = 0;
	while (j < _clients.size() && _clients[j].getFD() != clientFd)
		++j;
	_clients[j].joinChannel(name);


	/**
	 * @brief all rnf data that we send to hexchat to give context on the connexion
	 */
	// IRC standard: broadcast JOIN message to all channel members (including sender)
	std::ostringstream oss;
	oss << ":" << _clients[j].getName() << " JOIN " << name << "\r\n";
	std::string joinMsg = oss.str();
	it->second.broadcast(joinMsg);

	// Send topic (332) or no topic (331) to the joining client
	if (clientName.empty()) clientName = "*";
	if (!it->second.getTopic().empty()) {
		std::string tmsg = numRepChannel(332, clientName, name, it->second.getTopic());
		send(clientFd, tmsg.c_str(), tmsg.size(), 0);
	} else {
		std::string tmsg = numRepChannel(331, clientName, name, "");
		send(clientFd, tmsg.c_str(), tmsg.size(), 0);
	}

	// Send NAMES reply (353) and end of names (366)
	sendNames(*client, it->second, name);
}
