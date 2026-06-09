/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:49:01 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 11:02:27 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <sys/socket.h>
//#include <netinet/in.h>

int main (int ac, char **av)
{
    if (ac != 3)
        return 1;
    Server serv;
    serv.SetPort(atoi(av[1]));
    serv.SetPwd(atoi(av[2]));
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd <= -1)
        return 1;
    //setsockopt to config socket
    sockaddr addr;
    bind(
        fd,
        (sockaddr *)&addr,
        sizeof(addr)
    );    
    
        
    return 0;
}

