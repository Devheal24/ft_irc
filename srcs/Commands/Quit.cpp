#include "../../includes/Server.hpp"

void Server::CommandQuit(std::istringstream &iss, int clientFd)
{
	Client* client = getClientByFd(clientFd);
	
	for (size_t i = 0; i < _fds.size(); i++)
		if (_fds[i].fd == clientFd)
			_fds.erase(_fds.begin() + i);
	for (std::map<std::string, Channel>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->second.hasMember(clientFd))
		{
			client->setActiveChannel(it->second.getName());
			CommandClose(iss, clientFd);
		}
	}
	std::string msg = ":server 901 " + client->getName() + " " + client->getUsername() + "@localhost" + " :You are now logged out\r\n";
	send(clientFd, msg.c_str(), msg.size(), 0);
	removeClient(clientFd);
	display_status();
}
