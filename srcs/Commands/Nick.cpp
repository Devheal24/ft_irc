#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>

/**
 * @brief handler for NICK flag recv() from hexchat.
 * NICK -> (nickname input by user)
 * -- also check pwd and name validity here --
 */
bool Server::CommandNick(std::istringstream &iss, size_t selfIdx, int clientFd)
{
	std::string nick;
	iss >> nick;

	while (!nick.empty() && 
			(nick[nick.size() - 1] == '\r' || nick[nick.size() - 1] == '\n'))
		nick.resize(nick.size() - 1);


	//test norm
	if (nick.length() >= NICK_MAXL)
	{
		std::cout << "invalid length of nickname (>10)" << nick << std::endl;
		std::string msg =":server NOTICE :invalid length of nickname (>10)\r\n";
		send(clientFd, msg.c_str(), msg.length(), 0);
		return (true);
	}
	for (size_t i = 0; i < nick.length(); i++)
	{
		if (!isdigit(nick[i]) && !isalnum(nick[i]) && nick[i] != '_' && nick[i] != '-')
		{
			std::cout << "invalid character in nickname" << nick << std::endl;
			std::string msg =":server NOTICE :invalid character in nickname\r\n";
			send(clientFd, msg.c_str(), msg.length(), 0);
			return (true);
		}
	}
	
	_clients[selfIdx].setNick(nick);
	if (_clients[selfIdx].getFirstRegistered() == true)
	{
		std::string wmsg = numRep(001, _clients[selfIdx].getName());
		send(clientFd, wmsg.c_str(), wmsg.size(), 0);
		std::cout << "REGISTERED fd=" << clientFd << " nick=" << _clients[selfIdx].getName() << std::endl;
		_clients[selfIdx].setFirstRegistered(false);
	}

	size_t j = -1;
	while (++j < _clients.size())
	{
		if (_clients[j].getName() == nick && _clients[j].getFD() != clientFd)
		{
			std::cerr << "nickname already used !" << std::endl;
			std::string msg = numRep(433, nick);
			send(clientFd, msg.c_str(), msg.size(), 0);
			return false;
		}
	}

	std::cout << "NICK fd=" << clientFd << " nick=[" << nick << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;
	return true;
}
