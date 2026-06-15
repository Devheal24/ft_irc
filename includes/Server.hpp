#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

/**
 * @include <vector> : conteneur séquentiel utilisé pour clients
 * @include <map> : table (key->value) utilisée pour channels par nom
 * @include <poll.h> : interface poll() pour multiplexage
 */
#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include <vector>
#include <map>
#include <poll.h>

class Server {
private:
    int         _port;
    std::string _pwd;
    std::map<std::string, Channel>  _channels;
    std::vector< Client >           _clients;
    int                             _listen_fd;
    std::vector<struct pollfd>      fds;

    void CommandPrivMsg(std::istringstream &iss, std::string &token, size_t selfIdx, int clientFd);
    void CommandPass(std::istringstream &iss, size_t selfIdx, int clientFd);
    void CommandJoin(std::istringstream &iss, int clientFd);
    bool CommandNick(std::istringstream &iss, size_t selfIdx, int clientFd);
    bool CommandUser(std::istringstream &iss, size_t selfIdx, int clientFd);
    void CommandKick(std::istringstream &iss, int clientFD);
    void CommandInvite(std::istringstream &iss, int clientFD);
    void CommandTopic(std::istringstream &iss, int clientFD);
    void CommandMode(std::istringstream &iss, int clientFD);
    void kick(int clientFd, const std::string& channelName, const std::string& targetName, const std::string& reason);
    void invite(int clientFd, const std::string& targetNick, const std::string& channelName);
    void topic(int clientFd, const std::string& channelName, const std::string& newTopic);
public:

    Server() {};
    void SetPort (int port);
    int GetPort() const;
    void SetPwd (std::string pwd);
    std::string GetPwd() const;

    void display_status();

    bool parse_data(char **av);
    int init_server();
    void run_event_loop();
    bool handleClientInput(int clientFd);
    
    void joinChannel(int clientFd, const std::string& name);
    void removeClient(int clientFd);

    int getClientFdByName(const std::string& name) const;
    Client* getClientByFd(int fd);
    const Client* getClientByFd(int fd) const;
    std::string getClientPrefix(int fd) const;
};

# endif