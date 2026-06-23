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
	if (user.length() >= USR_MAXL)
	{
		std::cout << "invalid length of username (>20)" << user << std::endl;
		return (true);
	}
	for (size_t i = 0; i < user.length(); i++)
	{
		if (!isdigit(user[i]) && !isalnum(user[i]) && user[i] != '_' && user[i] != '-')
		{
			std::cout << "invalid character in username" << user << std::endl;
			return (true);
		}
	}

	//bool wasRegistered = _clients[selfIdx].isRegistered();
	_clients[selfIdx].setUser(user, real);

	size_t j = -1;
	while (++j < _clients.size())
	{
		if (_clients[j].getUsername() == user && _clients[j].getFD() != clientFd)
		{
			std::cerr << "Username already used !" << std::endl; return false;
		}
	}

	std::cout << "DEBUG USER fd=" << clientFd << " user=[" << user << "] real=[" << real << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;

	/*if (!wasRegistered && _clients[selfIdx].isRegistered())
	{
		std::string nick = _clients[selfIdx].getName();
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
	}*/
	return true;
}
