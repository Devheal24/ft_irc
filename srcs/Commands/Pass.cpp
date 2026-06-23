#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>

/**
 * @brief handler for PASS flag recv() from hexchat.
 * PASS -> (password input by user)
 */
void Server::CommandPass(std::istringstream &iss, size_t selfIdx, int clientFd)
{
	std::string pass;
	iss >> pass;

	while (!pass.empty() &&
			(pass[pass.size() - 1] == '\r' || pass[pass.size() - 1] == '\n'))
		pass.resize(pass.size() - 1);

	if (!pass.empty() && (pass.length() < PWD_MINL || pass.length() > PWD_MAXL))
	{
		std::cout << "invalid length of pass (64> || <8)" << pass << std::endl;
		std::string msg =":server NOTICE :invalid length of pass (64> || <8)\r\n";
		send(clientFd, msg.c_str(), msg.length(), 0);
		return;
	}
	for (size_t i = 0; i < pass.length(); i++)
	{
		if (!isprint(pass[i]) && pass[i] != '_' && pass[i] != '-')
		{
			std::cout << "invalid character in password" << pass << std::endl;
			std::string msg =":server NOTICE :invalid character in password\r\n";
			send(clientFd, msg.c_str(), msg.length(), 0);
			return;
		}
	}

	if (pass != _pwd)
	{
		std::cout << "passeword doesnt match server pwd" << pass << std::endl;
		std::string msg =":server NOTICE :password doesnt match server pwd\r\n";
		send(clientFd, msg.c_str(), msg.length(), 0);
		return;
	}

	_clients[selfIdx].setPass(pass);
	if (_clients[selfIdx].getHasName() && _clients[selfIdx].getHasNick() && (_clients[selfIdx].getHasPass() || _pwd.empty()))
			_clients[selfIdx].setRegistered(true);
	if (_clients[selfIdx].getFirstRegistered() == true  && _clients[selfIdx].isRegistered())
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

	std::cout << "PASS fd=" << clientFd << " pass=[" << pass << "]" << std::endl;
	return;
}
