/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:46:29 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/10 12:02:16 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERV
# define SERV

#include "Channel.hpp"
#include "../includes/Client.hpp"
#include <vector>
#include <map>

class Server {
private:
    int         _port;
    std::string _pwd;
    std::map<std::string, Channel>  _channels;
    std::vector< Client >           _clients;
    int                             _listen_fd;
public:

    Server() {};
    //get x Set
    void SetPort (int port);
    int GetPort() const;
    void SetPwd (std::string pwd);
    std::string GetPwd() const;

    //member-func
    bool parse_data(char **av);
    int init_server();
    void run_event_loop();
    bool handleClientInput(int clientFd);
    
    //client join
    void joinChannel(int clientFd, const std::string& name);
    void removeClient(int clientFd);

    //operator actions
    /*void kick(int clientFd, const std::string& channelName, const std::string& targetName, const std::string& reason);
    void invite(int clientFd, const std::string& targetNick, const std::string& channelName);
    void topic(int clientFd, const std::string& channelName, const std::string& newTopic);*/
};

# endif