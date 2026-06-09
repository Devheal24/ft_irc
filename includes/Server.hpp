/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:46:29 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 13:09:01 by jhubier          ###   ########.fr       */
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
    void SetPwd (std::string pwd);
    std::string GetPwd() const;

    //member-func
    bool parse_data(char **av);
    int init_server();
    
};

# endif