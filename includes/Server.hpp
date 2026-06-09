/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:46:29 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 10:58:58 by jhubier          ###   ########.fr       */
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
    void SetPort (int port);
    void GetPort () const;
    void SetPwd (int port);
    void GetPwd () const;
    
};

# endif