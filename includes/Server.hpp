/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimbow <jimbow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:46:29 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 14:00:40 by jimbow           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERV
# define SERV

#include <iostream>
#include "Channel.hpp"
#include <map>

class Server {
private:
    int         _port;
    std::string _pwd;
    std::map<std::string, Channel> _channels;
public:
    //get x Set
    void SetPort (int port);
    int GetPort() const;
    void SetPwd (std::string pwd);
    std::string GetPwd() const;

    //member-func
    bool parse_data(char **av);
    int init_server();
    
    //client join
    void joinChannel(int clientFd, const std::string& name);
};

# endif