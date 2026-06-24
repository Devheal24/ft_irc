#include "../../includes/Server.hpp"
#include <unistd.h>

void Server::CommandQuit(int clientFd, bool sigquit)
{
	Client* client = getClientByFd(clientFd);
	
	for (std::map<std::string, Channel>::const_iterator it = _channels.begin(); it != _channels.end();)
	{
		if (it->second.hasMember(clientFd))
		{
			std::string tmp = it->second.getName();
			++it;
			CommandPart(tmp, clientFd, sigquit);
		}
		else
			++it;
	}
	for (size_t i = 0; i < _fds.size(); i++)
		if (_fds[i].fd == clientFd)
			_fds.erase(_fds.begin() + i);
	if (sigquit == 0)
	{
		std::string msg = ":server 901 " + client->getName() + " " + client->getUsername() + "@localhost" + " :You are now logged out\r\n";
		send(clientFd, msg.c_str(), msg.size(), 0);
	}
	removeClient(clientFd);
	close(clientFd);
	display_status();
}
