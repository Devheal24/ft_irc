/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:49:01 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 12:54:00 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cstdlib> //atoi

int main (int ac, char **av)
{
    if (ac != 3) {std::cerr << "Error\n -> expected ./irc <port> <password>" << std::endl; return 1;}
    
    //parsing todo
    Server serv;
    serv.SetPort(atoi(av[1]));
    if (!serv.is_port_valid()) {return 1;};
    serv.SetPwd(atoi(av[2]));
    if (!serv.is_pwd_valid()) {return 1;};

    serv.init_server();
    return 0;
}