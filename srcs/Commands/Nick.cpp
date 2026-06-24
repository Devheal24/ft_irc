#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>
#include <set>

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
	if (nick.length() >= NICK_MAXL || nick.length() < NICK_MINL)
	{
		std::cout << "invalid length of nickname (>10)" << nick << std::endl;
		std::string msg =":server NOTICE :invalid length of nickname (10> x <1)\r\n";
		send(clientFd, msg.c_str(), msg.length(), 0);
		return (true);
	}
	for (size_t i = 0; i < nick.length(); i++)
	{
		if (!std::isprint(nick[i]) || nick[i] == '$' || nick[i] == ',' || nick[i] == '#')
		{
			std::cout << "invalid character in nickname" << nick << std::endl;
			std::string msg =":server NOTICE :invalid character in nickname\r\n";
			send(clientFd, msg.c_str(), msg.length(), 0);
			return (true);
		}
	}

	size_t j = -1;
	while (++j < _clients.size())
	{
		std::vector<Client>::iterator it;
		printComparativeClient(nick, it, _clients);
		if (_clients[j].getName() == nick && _clients[j].getFD() != clientFd)
		{
			std::cerr << "nickname already used !" << std::endl;
			std::string msg = numRep(433, nick);
			send(clientFd, msg.c_str(), msg.size(), 0);
			return false;
		}
	}

	std::string old_name = _clients[selfIdx].getName();
	_clients[selfIdx].setNick(nick);
	if (_clients[selfIdx].getHasName() && _clients[selfIdx].getHasNick() && (_clients[selfIdx].getHasPass() || _pwd.empty()))
			_clients[selfIdx].setRegistered(true);
	if (_clients[selfIdx].getFirstRegistered() == true && _clients[selfIdx].isRegistered())
	{
		std::string wmsg;
		for (int i = 1; i <= 4; i++)
		{
			wmsg = numRep(i, _clients[selfIdx].getName());
			send(clientFd, wmsg.c_str(), wmsg.size(), 0);
		}
		std::cout << "REGISTERED fd=" << clientFd << " nick=" << _clients[selfIdx].getName() << std::endl;
		_clients[selfIdx].setFirstRegistered(false);
	}

	if (_clients[selfIdx].getFirstRegistered() == false && nick != old_name)
	{
		std::string msg = ":" + old_name + "!" + _clients[selfIdx].getUsername() + " NICK " + nick + "\r\n";
		for (std::set<std::string>::const_iterator it = _clients[selfIdx].getJoinedChannels().begin(); it != _clients[selfIdx].getJoinedChannels().end() ; it ++)
		{
			for (std::vector<Client>::iterator ite = _clients.begin(); ite != _clients.end(); ite++)
				send(ite->getFD(), msg.c_str(), msg.size(), 0);
		}
	}

	std::cout << "NICK fd=" << clientFd << " nick=[" << nick << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;
	return true;
}
