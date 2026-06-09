/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:35 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 12:53:22 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <unistd.h>
//#include <sys/socket.h> //socket
#include <netinet/in.h> //maybe needeed for socket

/**
 * @brief all getter / setter
 */

int Server::GetPort() const {return _port;};
void Server::SetPort (int port) {
    _port = port;
}

std::string Server::GetPwd() const {return _pwd;};
void Server::SetPwd (int pwd) {
    _pwd = pwd;
}

//todo
/*static int set_nonblocking(int listen_fd)
{
    int flags = fcntl(listen_fd, F_GETFL, 0);      
    if (flags == -1)
        return -1;
    return fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK);
}*/

/**
 * @brief start the server listening on serv._port then loop with poll() and accept()
 * to always accept client connexion (no-lock)
 */
int Server::init_server()
{
    //"create" / "open" socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd <= -1)
        return 1;

    //set socket _port as instant reusable when closed
    int is_reuse = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &is_reuse, sizeof(is_reuse)) < 0)
        {std::cerr << "setsockpt fail" << std::endl; close(listen_fd); return 1;}

    //associate ip/port to the socket
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(this->GetPort());
    if (bind(listen_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        std::cerr << "bind fail" << std::endl;
    std::cerr << "bind succed" << std::endl;
    
    //wait for connection
    listen(listen_fd, SOMAXCONN);
    
    //handle client connection request
    sockaddr_in client_handler;
    socklen_t clientLen = sizeof(client_handler);
    if (accept(listen_fd, (sockaddr *)&serv_addr, &clientLen) < 0)
        std::cerr << "accept fail" << std::endl;
    std::cerr << "client connected" << std::endl;
    
    //will set all socket as nonblocking
    /*if (set_nonblocking(listen_fd) < 0) {
        std::cerr << "failed to set non-blocking" << std::endl;
        close(listen_fd);
        return 1;
    }*/
    return 0;
}

int Server::is_port_valid() {return 1;};
int Server::is_pwd_valid() {return 1;};