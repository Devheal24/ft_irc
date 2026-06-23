#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>

/**
 * @brief handler for USER flag recv() from hexchat.
 * USER -> (username input by user)
 */
bool Server::CommandUser(std::istringstream &iss, size_t selfIdx, int clientFd)
{
	std::string user, mode, unused;
	iss >> user >> mode >> unused;
	std::string real;
	std::getline(iss, real);

	if (!real.empty() && real[0] == ' ')
		real.erase(0, 1);
	if (!real.empty() && real[0] == ':')
		real.erase(0, 1);

	while (!real.empty() &&
			(real[real.size() - 1] == '\r' || real[real.size() - 1] == '\n'))
		real.resize(real.size() - 1);

	
	//test norm
	if (user.length() >= USR_MAXL || user.length() < USR_MINL)
	{
		std::cout << "invalid length of username (>20)" << user << std::endl;
		std::string msg =":server NOTICE :invalid length of username (>20)\r\n";
		send(clientFd, msg.c_str(), msg.length(), 0);
		return (true);
	}
	for (size_t i = 0; i < user.length(); i++)
	{
		if (!isdigit(user[i]) && !isalnum(user[i]) && user[i] != '_' && user[i] != '-')
		{
			std::cout << "invalid character in username" << user << std::endl;
			std::string msg =":server NOTICE :invalid character in username\r\n";
			send(clientFd, msg.c_str(), msg.length(), 0);
			return (true);
		}
	}

	_clients[selfIdx].setUser(user, real);
	if (_clients[selfIdx].getHasName() && _clients[selfIdx].getHasNick() && (_clients[selfIdx].getHasPass() || _pwd.empty()))
			_clients[selfIdx].setRegistered(true);
	if (_clients[selfIdx].getFirstRegistered() == true  && _clients[selfIdx].isRegistered())
	{
		std::string wmsg = numRep(001, _clients[selfIdx].getName());
		send(clientFd, wmsg.c_str(), wmsg.size(), 0);
		std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << _clients[selfIdx].getName() << std::endl;
		_clients[selfIdx].setFirstRegistered(false);
	}

	size_t j = -1;
	while (++j < _clients.size())
	{
		if (_clients[j].getUsername() == user && _clients[j].getFD() != clientFd)
		{
			std::cerr << "Username already used !" << std::endl; return false;
			std::string msg =":server NOTICE :username already used !\r\n";
			send(clientFd, msg.c_str(), msg.length(), 0);
		}
	}

	std::cout << "DEBUG USER fd=" << clientFd << " user=[" << user << "] real=[" << real << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;
	return true;
}
