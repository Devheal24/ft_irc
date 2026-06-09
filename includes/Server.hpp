/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:46:29 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 12:52:00 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERV
# define SERV

#include <iostream>

class Server {
private:
    int         _port;
    std::string _pwd;
public:
    //get x Set
    void SetPort (int port);
    int GetPort() const;
    void SetPwd (int pwd);
    std::string GetPwd() const;

    //member-func
    int init_server();
    int is_port_valid();
    int is_pwd_valid();
    
};

# endif