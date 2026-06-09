/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:49:01 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 11:38:36 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cstdlib> //atoi
//#include <sys/socket.h> //socket
#include <netinet/in.h> //maybe needeed for socket

int main (int ac, char **av)
{
    //parsing
    if (ac != 3) {std::cerr << "not enougth arg" << std::endl; return 1;}
    Server serv;
    serv.SetPort(atoi(av[1]));
    serv.SetPwd(atoi(av[2]));

    //socket set
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd <= -1)
        return 1;

    int yes = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        {std::cerr << "setsockpt fail" << std::endl; return 1;}    

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(serv.GetPort());
    if (bind(fd, (sockaddr *)&addr, sizeof(addr)) < 0)
        std::cerr << "bind fail" << std::endl;
    std::cerr << "bind succed" << std::endl;
    
    listen(fd, SOMAXCONN);
    //check

    sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    if (accept(fd, (sockaddr *)&addr, &clientLen) < 0)
        std::cerr << "accept fail" << std::endl;
    std::cerr << "client connected" << std::endl;

    
    
    return 0;
}

