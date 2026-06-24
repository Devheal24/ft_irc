#include "../../includes/Server.hpp"
#include <iostream>

void Server::CommandPart(std::string chan, int clientFd, bool sigquit)
{
	size_t j = 0;

	if (!chan.empty() && chan[0] == ':')
		chan = chan.substr(1);

	while (j < _clients.size() && _clients[j].getFD() != clientFd)
		++j;
	if (j == _clients.size())
		return;
	std::map<std::string, Channel>::iterator cit = _channels.find(chan);
	if (cit == _channels.end())
	{
		std::string msg = numRepChannel(403, _clients[j].getName(), chan, "");
		send(clientFd, msg.c_str(), msg.size(), 0);
		return;
	}
	cit->second.removeMember(clientFd);
	cit->second.removeOperator(clientFd);
	std::string msg = ":" + _clients[j].getName() + "!" + _clients[j].getUsername() + "@localhost" + " PART " + cit->second.getName() + "\r\n";
	cit->second.broadcast(msg.c_str());
	if (sigquit == 0)
		send(clientFd, msg.c_str(), msg.size(), 0);
	if (cit->second.shouldClose(-1))
	{
		_channels.erase(cit);
		std::cout << "channel erase (0 member)"  << std::endl;
	}
}
