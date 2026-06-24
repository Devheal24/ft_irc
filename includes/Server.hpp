#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

/**
 * @include <vector> : container use for client check
 * @include <map> : table (key->value) use for check channel's name
 * @include <poll.h> : interface poll() for multiplexage check
 */
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include <vector>
#include <map>
#include <poll.h>
#include <algorithm>

/**
 * @brief all user data "inputs" max length.
 */
#define PWD_MAXL 64
#define PWD_MINL 8
#define USR_MAXL 20
#define USR_MINL 1
#define NICK_MAXL 20
#define NICK_MINL 1
#define CHNL_MAXL 20
#define CHNL_MINL 1


class Server {
private:
	int								_port;
	int								_listen_fd;
	std::string						_pwd;
	std::vector< Client >			_clients;
	std::vector<struct pollfd>		_fds;
	std::map<std::string, Channel>	_channels;
	std::map<int, std::string>		_data;

	// Users Commands
	void CommandPrivMsg(std::istringstream &iss, std::string &token, size_t selfIdx, int clientFd);
	void CommandPass(std::istringstream &iss, size_t selfIdx, int clientFd);
	void CommandJoin(std::istringstream &iss, int clientFd);
	bool CommandNick(std::istringstream &iss, size_t selfIdx, int clientFd);
	bool CommandUser(std::istringstream &iss, size_t selfIdx, int clientFd);
	void CommandPart(std::string chan, int clientFd, bool sigquit);
	void CommandQuit(int clientFd, bool sigquit);
	// Operators Commands
	void CommandKick(std::istringstream &iss, int clientFd);
	void CommandInvite(std::istringstream &iss, int clientFd);
	void CommandTopic(std::istringstream &iss, int clientFd);
	void CommandMode(std::istringstream &iss, int clientFd);
	void kick(int clientFd, std::string& channelName, std::string& targetName, const std::string& reason);
	void invite(int clientFd, std::string& targetNick, std::string& channelName);
	void topic(int clientFd, std::string& channelName, std::string& newTopic);
	// Utilities functions
	std::vector<std::string> splitComma(const std::string &name);

public:
	Server() {};

	void			SetPort (int port);
	int			 	GetPort() const;
	void			SetPwd (std::string pwd);
	std::string	 	GetPwd() const;

	void			display_status();

	bool			parse_data(char **av);
	int				init_server();
	void			run_event_loop();
	bool			handleClientInput(int clientFd);

	void			joinChannel(int clientFd, std::string& name, const std::string& key);
	void			removeClient(int clientFd);

	int			 	getClientFdByName(const std::string& name) const;
	Client*		 	getClientByFd(int fd);
	const Client*   getClientByFd(int fd) const;
	Client*			getClientbyName(const std::string& name);
	std::string	 	getClientPrefix(int fd) const;

	void			createBot();
	Client*			getBot();

	void			sendNames(Client& client, Channel& channel, const std::string & channelName);


	template<typename T, typename L>
	void printComparativeChannel(std::string& name, T& it, L& list)
	{
		std::string lower_name = name;
		std::string lower_name_target;
		std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
	
		T lower_target;

		for (lower_target = list.begin(); lower_target !=  list.end(); ++lower_target)
		{
			lower_name_target = lower_target->first;
			std::transform(lower_name_target.begin(), lower_name_target.end(), lower_name_target.begin(), ::tolower);
		
			if (lower_name == lower_name_target)
			{
				it = lower_target;
				name = lower_target->first;
				return;
			}
		}
		it = lower_target;
		return;
	}


	template<typename T, typename L>
	void printComparativeClient(std::string& name, T& it, L& list)
	{
		std::string lower_name = name;
		std::string lower_name_target;
		std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);

		T lower_target;

		for (lower_target = list.begin(); lower_target !=  list.end(); ++lower_target)
		{
			lower_name_target = lower_target->getName();
			std::transform(lower_name_target.begin(), lower_name_target.end(), lower_name_target.begin(), ::tolower);

			if (lower_name == lower_name_target)
			{
				it = lower_target;
				name = lower_target->getName();
				return;
			}
		}
		it = lower_target;
		return;
	}
};

std::string		 	numRep(int code, const std::string& nick);
std::string		 	numRepChannel(int code, const std::string& nick, const std::string& channel, const std::string& addon);

# endif
