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

	if (!pass.empty() && (pass.length() < PWD_MINXL || pass.length() > PWD_MAXL))
	{
		std::cout << "invalid length of pass (64> || <8)" << pass << std::endl;
		return;
	}
	for (size_t i = 0; i < pass.length(); i++)
	{
		if (!isprint(pass[i]) && pass[i] != '_' && pass[i] != '-')
		{
			std::cout << "invalid character in passname" << pass << std::endl;
			return;
		}
	}

	/*if (pass != _pwd)
	{
		std::cout << "passeword doesnt match server pwd" << pass << std::endl;
		return;
	}*/

	_clients[selfIdx].setPass(pass);

	std::cout << "DEBUG PASS fd=" << clientFd << " pass=[" << pass << "]" << std::endl;
	return;
}
