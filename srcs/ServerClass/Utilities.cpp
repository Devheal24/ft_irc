#include "../../includes/Server.hpp"
#include <sstream>
#include <fcntl.h>

/**
 * @brief when client quit irc, close/clean correctly all of his appearance in container/else
 */
void Server::removeClient(int clientFd)
{
	//display_status();
	size_t j = 0;
	while (j < _clients.size() && _clients[j].getFD() != clientFd)
		++j;
	if (j == _clients.size())
		return;

	const std::set<std::string>& chans = _clients[j].getJoinedChannels();
	for (std::set<std::string>::const_iterator it = chans.begin(); it != chans.end(); ++it)
	{
		std::map<std::string, Channel>::iterator cit = _channels.find(*it);
		if (cit != _channels.end())
		{
			cit->second.removeMember(clientFd);
			cit->second.removeOperator(clientFd);
			if (cit->second.shouldClose(-1))
			{
				_channels.erase(cit);
				std::cout << "channel erase (0 member)"  << std::endl;
			}
		}
	}

	_data.erase(clientFd);
	_clients.erase(_clients.begin() + j);
	display_status();
}

/**
 * @brief Set the client to nonblocking to still loop if no new input detected
 */
int Server::set_nonblocking(int fd)
{
	return fcntl(fd, F_SETFL, O_NONBLOCK);
}

std::vector<std::string> Server::splitComma(const std::string &name)
{
	std::vector<std::string> tmp;
	std::stringstream iss(name);
	std::string buf;
	while (getline(iss, buf, ','))
		tmp.push_back(buf);
	return (tmp);
}

char Server::normalize(char s)
{
	s = std::tolower(static_cast<unsigned char>(s));
	if (s == '{')
		s = '[';
	else if (s == '}')
		s = ']';
	else if (s == '|')
		s = '\\';
	return (s);
}
