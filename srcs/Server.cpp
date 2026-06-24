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
#include "../includes/Server.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdlib>
#include <fcntl.h>
#include <vector>
#include <cerrno>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>

extern int g_sig;

/**
 * @brief all getter / setter
 */
int Server::GetPort() const {return _port;};
void Server::SetPort (int port) {
	_port = port;
}
std::string Server::GetPwd() const {return _pwd;};
void Server::SetPwd (std::string pwd) {
	_pwd = pwd;
}

/**
 * @brief get User input and handle/parse them
 */
bool Server::parse_data(char **av) {
	//port parsing
	char *end = NULL;
	this->SetPort(std::strtol(av[1], &end, 10));
	if (GetPort() == 0 || end == av[1] || *end != '\0' || GetPort() < 6665 || GetPort() > 6669)
	{
		std::cerr << "Error\n -> port parsing : " << av[1] << std::endl;
		return false;  
	}

	//pwd parsing
	this->SetPwd(static_cast<std::string>(av[2]));
	if (!_pwd.empty() && (_pwd.length() < PWD_MINL || _pwd.length() > PWD_MAXL))
	{
		std::cout << "invalid length of pass (64> || <8)" << _pwd << std::endl;
		return false;
	}
	for (size_t i = 0; i < _pwd.length(); i++)
	{
		if (!isprint(_pwd[i]))
		{
			std::cout << "invalid character in passname" << _pwd << std::endl;
			return false;
		}
	}
	return true;
};

/**
 * @brief Set the client to nonblocking to still loop if no new input detected
 */
static int set_nonblocking(int fd)
{
	return fcntl(fd, F_SETFL, O_NONBLOCK);
}

void Server::display_status() {
	std::cout << std::endl << "SERVER STATE:" << std::endl;
	for (int i = 0; i < (int)_clients.size(); i++)
		std::cout << "client n°" << i << " == fd[" << _clients[i].getFD() << "] :ip[" << _clients[i].getIP() << "] :name[" <<_clients[i].getName() <<"] :pwd[" << _clients[i].getPass() << "] :user[" << _clients[i].getUsername() << "]" << std::endl;
	std::cout << std::endl << std::endl;
}

/**
 * @brief Create, bind, listen and set non-blocking a listening socket for given port
 */
int Server::init_server()
{
	srand(time(NULL));

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		std::cerr << "socket fail" << std::endl;
		return 1;
	}

	int is_reuse = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &is_reuse, sizeof(is_reuse)) < 0)
	{
		std::cerr << "setsockopt fail" << std::endl;
		close(listen_fd);
		return 1;
	}

	sockaddr_in serv_addr;
	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(_port);

	if (bind(listen_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cerr << "bind fail" << std::endl;
		close(listen_fd);
		return 1;
	}

	if (listen(listen_fd, SOMAXCONN) < 0)
	{
		std::cerr << "listen fail" << std::endl;
		close(listen_fd);
		return 1;
	}

	if (set_nonblocking(listen_fd) < 0)
	{
		std::cerr << "failed to set non-blocking" << std::endl;
		close(listen_fd);
		return 1;
	}

	_listen_fd = listen_fd;
	return 0;
}

/**
 * @brief Run the main "infinite" poll() event loop on listening fd.
 */
void Server::run_event_loop()
{
	struct pollfd listen_pollfd;
	listen_pollfd.fd = _listen_fd;
	listen_pollfd.events = POLLIN;
	listen_pollfd.revents = 0;
	_fds.push_back(listen_pollfd);

	std::cout << "Server port: " << _port << std::endl;
	std::cout << "Server password: " << _pwd << std::endl;
	std::cout << "Server listening ..." << std::endl;

	while (!g_sig)
	{
		int ready = poll(&_fds[0], _fds.size(), -1);
		if (ready <= 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "poll fail" << std::endl;
			break;
		}

		// incoming connections
		if (_fds[0].revents & POLLIN)
		{
			while (!g_sig)
			{
				sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_fd = accept(_listen_fd, (sockaddr *)&client_addr, &client_len);
				if (client_fd < 0)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						break;
					if (errno == EINTR)
						continue;
					std::cerr << "accept fail" << std::endl;
					break;
				}

				if (set_nonblocking(client_fd) < 0)
				{
					close(client_fd);
					continue;
				}
				std::string ip = inet_ntoa(client_addr.sin_addr);
				struct pollfd client_pollfd;
				client_pollfd.fd = client_fd;
				client_pollfd.events = POLLIN;
				client_pollfd.revents = 0;
				_fds.push_back(client_pollfd);
				// create placeholder client name so the client appears registered to hexChat
				std::ostringstream cn;
				cn << "client" << client_fd;
				std::string placeholderName = cn.str();
				_clients.push_back(Client(std::string(""), client_fd, ip));
				std::cout << "client connected fd " << client_fd << " with ip= " << ip << std::endl;
				display_status();
			}
		}
		// events on client sockets
		for (size_t i = 1; i < _fds.size(); ++i)
		{
			short revents = _fds[i].revents;
			if (revents == 0)
				continue;

			int clientFd = _fds[i].fd;
			if (revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				std::cout << "client disconnected POLLHUP fd " << clientFd << std::endl;
				// removeClient(clientFd);
				CommandQuit(clientFd, 1);
				// _fds.erase(_fds.begin() + i);
				// --i;
				break;
			}
			// if a client have send something we handle what we received
			if (revents & POLLIN)
			{
				bool connected = handleClientInput(clientFd);
				if (!connected)
				{
					std::cout << "client disconnected POLLIN fd " << clientFd << std::endl;
					// removeClient(clientFd);
					CommandQuit(clientFd, 1);
					// _fds.erase(_fds.begin() + i);
					// --i;
					break;
				}
			}
		}
	}

	//close all fd when server shutdown
	for (size_t i = 0; i < _fds.size(); ++i)
		close(_fds[i].fd);

	for (size_t i = 0; i < _fds.size(); ++i)
	{
		std::cout << "connected at close : " << _fds[i].fd << std::endl;
	}
}

/**
 * @brief handle each client input, look for special cmd and send broadcast correctly
 */
bool Server::handleClientInput(int clientFd)
{
	char buf[1024];
	std::memset(buf, 0, sizeof(buf));
	ssize_t n = recv(clientFd, buf, sizeof(buf), 0);
	if (n <= 0)
		return false;
	_data[clientFd].append(buf, n);
	if (_data[clientFd].find("\n") == std::string::npos)
		return true;
	std::string data = _data[clientFd];
	_data[clientFd] = "";

	size_t selfIdx = 0;
	while (selfIdx < _clients.size() && _clients[selfIdx].getFD() != clientFd)
		++selfIdx;

	// split data into lines by LF, trim CR, and process each line
	std::vector<std::string> lines;
	std::string cur;
	for (size_t i = 0; i < data.size(); ++i) {
		char c = data[i];
		if (c == '\n') {
			if (!cur.empty() && cur[cur.size() - 1] == '\r')
				cur.resize(cur.size() - 1);
			lines.push_back(cur);
			cur.clear();
		} else {
			cur.push_back(c);
		}
	}
	if (!cur.empty()) {
		if (cur[cur.size() - 1] == '\r')
			cur.resize(cur.size() - 1);
		lines.push_back(cur);
	}

	for (size_t li = 0; li < lines.size(); ++li) {
		std::string &line = lines[li];
		if (line.empty())
			continue;
		std::istringstream iss(line);
		std::string token;
		iss >> token;

		if (token == "PASS" || token == "/PASS")
		{
			CommandPass(iss, selfIdx, clientFd);
			continue;
		}
		if (token == "NICK" || token == "/NICK")
		{
			if (CommandNick(iss, selfIdx, clientFd) == false)
				return false;
			continue;
		}
		if (token == "USER" || token == "/USER")
		{
			if (CommandUser(iss, selfIdx, clientFd) == false)
				return false;
			continue;
        }
        if (token == "QUIT" || token == "/QUIT")
		{
			CommandQuit(clientFd, 0);
			continue;
		}

		//check if nick+user set (pwd optional)
		int i;
		for (i = 0; i < (int)_clients.size(); ++i)
		{
			if (_clients[i].getFD() == clientFd)
				break;
		}
		if (!_clients[i].isRegistered())
		{
			std::cout << _clients[i].getName() << std::endl;
			std::cout << _clients[i].getUsername() << std::endl;
			std::cerr << "client is not fully logged-in" << std::endl;
            std::string msg =":server NOTICE :you are not fully logged in (PASS/USER/NICK)\r\n";
			send(clientFd, msg.c_str(), msg.length(), 0);
			continue;
		}

		if (token == "JOIN" || token == "/JOIN")
		{
			CommandJoin(iss, clientFd);
			continue;
		}
		if (token == "PRIVMSG" || token == "NOTICE")
		{
			CommandPrivMsg(iss, token, selfIdx, clientFd);
			continue;
		}
		if (token == "KICK" || token == "/KICK")
		{
			CommandKick(iss, clientFd);
			continue;   
		}
		if (token == "INVITE" || token == "/INVITE")
		{
			CommandInvite(iss, clientFd);
			continue;   
		}
		if (token == "TOPIC" || token == "/TOPIC")
		{
			CommandTopic(iss, clientFd);
			continue;   
		}
		if (token == "MODE" || token == "/MODE")
		{
			CommandMode(iss, clientFd);
			continue;
		}
		if (token == "PART" || token == "/PART")
		{
			CommandClose(clientFd);
			continue;
		}
		std::cout << "IGNORED fd=" << clientFd << " line=[" << line << std::endl << std::endl;
	}
	return true;
}

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

void Server::sendNames(Client& client, Channel& channel, const std::string & channelName)
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
