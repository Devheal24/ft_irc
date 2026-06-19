#include "../../includes/Server.hpp"
#include <sstream>
#include <iostream>

void Server::CommandClose(std::istringstream &iss, int clientFd)
{
	/*std::string tmp;
	iss >> tmp;
	if (!tmp.empty())
	{
		std::cout << "close syntaxe error" << std::endl;
		return;
	}*/
	(void)iss;

	std::cerr << "close cmd used " << std::endl;

	size_t j = 0;
	while (j < _clients.size() && _clients[j].getFD() != clientFd)
		++j;
	if (j == _clients.size())
		return;

	const std::string& chans = _clients[j].getActiveChannel();
	std::map<std::string, Channel>::iterator cit = _channels.find(chans);
	cit->second.removeMember(clientFd);
	cit->second.removeOperator(clientFd);
	std::string msg = ":" + _clients[j].getName() + "!" + _clients[j].getUsername() + "@localhost" + " PART " + cit->second.getName() + "\r\n";
	send(clientFd, msg.c_str(), msg.size(), 0);
	cit->second.broadcast(msg.c_str());
	if (cit->second.memberCount() == 0)
	{
		_channels.erase(cit);
		std::cout << "channel erase (0 member)"  << std::endl;
	}

}
