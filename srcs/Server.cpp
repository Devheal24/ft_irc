/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jimbow <jimbow@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:06:35 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 15:30:00 by jimbow           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <unistd.h>
#include <netinet/in.h> //maybe needeed for socket
#include <cstdlib> //atoi
#include <sys/socket.h> //pour send();

/**
 * @brief all getter / setter
 */

int Server::GetPort() const {return _port;};
void Server::SetPort (int port) {
    _port = port;
}

std::string Server::GetPwd() const {return _pwd;};
void Server::SetPwd (std::string pwd) {
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
    int clientSocket = accept(listen_fd, (sockaddr *)&serv_addr, &clientLen);
    if (clientSocket < 0)
        std::cerr << "accept fail" << std::endl;
    std::cerr << "client connected" << std::endl;
    
    //will set all socket as nonblocking
    /*if (set_nonblocking(listen_fd) < 0) {
        std::cerr << "failed to set non-blocking" << std::endl;
        close(listen_fd);
        return 1;
    }*/

    close (listen_fd);
    close (clientSocket);
    return 0;
}

/**
 * @brief get User input and handle/parse them
 */
bool Server::parse_data(char **av) {
    //port parsing
    char *end = NULL;
    this->SetPort(std::strtol(av[1], &end, 10));
    if (GetPort() == 0 || end == av[1] || !(*end == '\0' || *end == '\n' || *end == '\r'))
    {
        std::cerr << "Error\n -> port parsing : " << av[1] << std::endl;
        return false;  
    }
    std::cout << "Debug _port : " << GetPort() << std::endl;

    //pwd parsing
    this->SetPwd((std::string)av[2]);
    //if (! valid pwd) {return0 false;};
    std::cout << "Debug _port : " << GetPwd() << std::endl;
    return true;
};


void Server::joinChannel(int clientFd, const std::string& name)
{
    std::map<std::string, Channel>::iterator it;

    it = _channels.find(name);

    if (it == _channels.end())
    {
        _channels.insert(std::make_pair(name, Channel(name)));
        it = _channels.find(name);
        std::cout << "Channel created: " << name << std::endl;
    }
    
    //firstMember is operator, else not
    bool firstMember = (_channels[name].memberCount() == 0);
    it->second.addMember(clientFd);
    if (firstMember)
    {
        _channels[name].addOperator(clientFd);
        std::cout << "Client " << clientFd << " is operator" << std::endl;
    }
    std::cout << "Client " << clientFd << " joined " << name << std::endl;
}

void Server::kick(int clientFd, const std::string& channelName, const std::string& targetName, const std::string& reason)
{
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    //verify if channel exist
    if (it == _channels.end())
    {
        std::string msg = ":server 403 " + channelName + " does not exist\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    //verify if client is operator
    Channel& ch = it->second;
    if (!ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + channelName + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    ////// a decommenter lorsque la classe Client sera cree
    //// Verify if target exist and is in channel
    // std::map<int, Client> _clients;

    // int targetFd = findClientFdByNick(targetName);
    // if (targetFd == clientFd)
    // {
    //     send(clientFd, "You can't kick yourself\r\n", 26, 0);
    //     return ;
    // }
    // if (!ch.hasMember(targetFd))
    // {
    //     std::string msg = targetName + " is not in the channnel\r\n";
    //     send(clientFd, msg.c_str(), msg.size(), 0);
    //     return ;
    // }

    // ch.removeMember(targetFd);
    // ch.removeOperator(targetFd);

    // std::string msg = ":" + getClientName(clientFd) + " KICK " + channelName + " " + targetName + " :" + reason + "\r\n";
    // ch.broadcast(msg);
}

void Server::invite(int clientFd, const std::string& targetNick, const std::string& channelName)
{
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);

    //verify if channel exist
    if (it == _channels.end())
    {
        std::string msg = ":server 403 " + channelName + " does not exist\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    Channel& ch = it->second;

    //verify if client is operator
    if (!ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + channelName + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // // Verify if target exist
    // int targetFd = findClientByNick(targetNick);
    // if (targetFd == -1)
    // {
    //     std::string msg = ":server 401 " + targetNick + " :user does not exist\r\n";
    //     send(clientFd, msg.c_str(), msg.size(), 0);
    //     return ;
    // }
}