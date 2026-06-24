/**
 * @include <iostream> : (std::cout / std::cerr)
 * @include <unistd.h> : functions POSIX (close, read, write, etc.)
 * @include <netinet/in.h> : (sockaddr_in) for bind/accept
 * @include <cstdlib> : atoi
 * @include <fcntl.h> : set files (fcntl) for setting non-blocking
 * @include <vector> : contenor for pollfd lst
 * @include <cerrno> : error code POSIX (errno) for accept() or revents
 * @include <cstring> : C memorie manipulation
 * @include <arpa/inet.h> : addr web conversion(htons, inet_*) for addr_in (port
 * @include <sstream> : flux for string
 * @include <sys/socket.h> : API sockets (socket, bind, listen, accept, send)
 */
#include "../../includes/Server.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdlib>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>


/**
 * @brief all getter / setter
 */
int Server::GetPort() const 
{
	return _port;
}

void Server::SetPort (int port)
{
	_port = port;
}

std::string Server::GetPwd() const
{
	return _pwd;
}

void Server::SetPwd (std::string pwd) {
	_pwd = pwd;
}

int Server::getClientFdByName(const std::string& name) const
{
	for (std::vector<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getName() == name)
			return it->getFD();
	}
	return -1;
}

Client* Server::getClientByFd(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getFD() == fd)
			return &(*it);
	}
	return NULL;
}

const Client* Server::getClientByFd(int fd) const
{
	for (std::vector<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getFD() == fd)
			return &(*it);
	}
	return NULL;
}

std::string Server::getClientPrefix(int fd) const
{
	const Client* client = getClientByFd(fd);

	if (client == NULL)
		return "unknown!unknown@localhost";
	
	return client->getName() + "!" + client->getUsername() + "@localhost";
}

Client* Server::getClientbyName(const std::string& name)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getName() == name)
			return &_clients[i];
	}
	return NULL;
}

void Server::display_status()
{
	std::cout << std::endl << "SERVER STATE:" << std::endl;
	for (int i = 0; i < (int)_clients.size(); i++)
		std::cout << "client n°" << i << " == fd[" << _clients[i].getFD() << "] :ip[" << _clients[i].getIP() << "] :name[" <<_clients[i].getName() <<"] :pwd[" << _clients[i].getPass() << "] :user[" << _clients[i].getUsername() << "]" << std::endl;
	std::cout << std::endl << std::endl;
}

void Server::createBot()
{
	Client bot("Bot", -1, "");
	bot.setBot(true);

	_clients.push_back(bot);
}

Client* Server::getBot()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getIsBot())
			return &_clients[i];
	}
	return NULL;
}

void Server::sendNames(Client& client, Channel& channel, const std::string& channelName)
{
	std::ostringstream names;

	for (size_t kk = 0; kk < _clients.size(); ++kk) 
	{
		int memberFd = _clients[kk].getFD();
	
		if (!channel.hasMember(memberFd))
			continue;
		
		std::string mname = _clients[kk].getName();
		if (mname.empty())
			mname = "*";
		
		if (channel.isOperator(memberFd))
			mname = "@" + mname;
		
		std::cout << "NAMES member fd=" << memberFd << " name=[" << _clients[kk].getName() << "] usedName=[" << mname << "]" << std::endl;
		names << mname;

		// detect if more members exist after kk
		bool more = false;
		
		for (size_t kk2 = kk + 1; kk2 < _clients.size(); ++kk2)
		{
			if (channel.hasMember(_clients[kk2].getFD()))
			{
				more = true;
				break;
			}
		}
		if (more)
			names << ' ';
	}
	std::string r353s = numRepChannel(353, client.getName(), channelName, names.str());
	send(client.getFD(), r353s.c_str(), r353s.size(), 0);

	std::string r366s = numRepChannel(366, client.getName(), channelName, "");
	send(client.getFD(), r366s.c_str(), r366s.size(), 0);	
}
