#include "../../includes/Server.hpp"
#include <cstring>
#include <sstream>

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
			CommandNick(iss, selfIdx, clientFd);
			continue;
		}
		if (token == "USER" || token == "/USER")
		{
			CommandUser(iss, selfIdx, clientFd);
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
			std::string command;
			iss >> command;
			CommandPart(command, clientFd, 0);
			continue;
		}
		std::cout << "IGNORED fd=" << clientFd << " line=[" << line << std::endl << std::endl;
	}
	return true;
}
