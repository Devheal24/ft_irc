/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:46:29 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 16:21:27 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERV
# define SERV

#include "Channel.hpp"
#include <map>

class Server {
private:
    int         _port;
    std::string _pwd;
    std::map<std::string, Channel> _channels;
    int         _listen_fd;
public:
    //get x Set
    void SetPort (int port);
    int GetPort() const;
    void SetPwd (std::string pwd);
    std::string GetPwd() const;

    //member-func
    bool parse_data(char **av);
    int init_server();
    void run_event_loop();
    
    //client join
    void joinChannel(int clientFd, const std::string& name);
};

# endif