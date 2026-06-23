#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

/**
 * @include <string> : stockage des noms et opérations sur chaînes check
 * @include <set> : structure pour membres/operateurs/invitations sans doublons check
 */
#include <arpa/inet.h>
#include <string>
#include <set>

class Client
{
	private:
		std::string			 	_name;
		std::string			 	_ip;
		int					 	_fd;

		std::set<std::string>   _joinedChannels;
		std::string			 	_activeChannel;

		std::string				_username;
		std::string				_realname;
		std::string				_pass;
		bool					_hasPass;
		bool					_hasNick;
		bool					_hasUser;
		bool					_registered;
		bool					_isBot;
		bool					_firstregistered;

	
	public:
		Client();
		Client(std::string name, int fd, std::string ip);
		~Client();
		Client(const Client &base);
		Client &operator=(const Client &base);

		void addClient(std::string name, int fd);

		// channel management
		void joinChannel(const std::string& channelName);
		void partChannel(const std::string& channelName);
		void setActiveChannel(const std::string& channelName);

		// registration
		void 		setNick(const std::string& nick);
		void 		setUser(const std::string& user, const std::string& real);
		void 		setPass(const std::string& pass);
		void 		setRegistered(bool state);
		void		setFirstRegistered(bool state);
		void 		setFD(int fd);
		bool 		isRegistered() const;
		std::string getPass() const;
		bool		getFirstRegistered();
		bool 		hasPass() const;

		std::string 					getActiveChannel() const;
		bool							isInChannel(const std::string& channelName) const;
		const std::set<std::string>&	getJoinedChannels() const;

		std::string getName() const;
		std::string getUsername() const;
		int		 	getFD() const;
		std::string getIP() const;
		bool		getHasNick() const;
		bool		getHasName() const;
		bool		getHasPass() const;

		bool		getIsBot() const;
		void		setBot(bool value);
};

#endif
