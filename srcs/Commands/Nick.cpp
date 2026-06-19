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
		return (true);
	}
	for (size_t i = 0; i < nick.length(); i++)
	{
		if (!isdigit(nick[i]) && !isalnum(nick[i]) && nick[i] != '_' && nick[i] != '-')
		{
			std::cout << "invalid character in nickname" << nick << std::endl;
			return (true);
		}
	}
	
	bool wasRegistered = _clients[selfIdx].isRegistered();
	_clients[selfIdx].setNick(nick);

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

	std::cout << "DEBUG NICK fd=" << clientFd << " nick=[" << nick << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;

	if (!wasRegistered && _clients[selfIdx].isRegistered())
	{
		bool needPass = !_pwd.empty();
		bool hasPass = !_clients[selfIdx].getPass().empty();
		if (needPass != hasPass || _clients[selfIdx].getPass() != _pwd)
		{
			std::string msg = numRep(464, nick);
			send(clientFd, msg.c_str(), msg.size(), 0);
			std::cout << "DEBUG REGISTRATION FAILED fd=" << clientFd << " nick=" << nick << " (bad PASS) - disconnecting" << std::endl;
			return false; // disconnect client on failed registration
		}
		else
		{
			std::string wmsg = numRep(001, nick);
			send(clientFd, wmsg.c_str(), wmsg.size(), 0);
			std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << nick << std::endl;
		}
	}
	return true;
}
