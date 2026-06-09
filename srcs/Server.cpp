#include "../includes/Server.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdlib>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <cerrno>
#include <cstring>

static int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
#include <sys/socket.h> //pour send();

// Create, bind, listen and set non-blocking a listening socket for given port.
// Returns listening fd on success, -1 on failure (and prints an error).
int Server::init_server()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        std::cerr << "socket fail" << std::endl;
        return 1;
    }

    int is_reuse = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &is_reuse, sizeof(is_reuse)) < 0)
    {
        std::cerr << "setsockopt fail" << std::endl;
        close(listen_fd);
        return 1;
    }

    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(_port);

    if (bind(listen_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "bind fail" << std::endl;
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, SOMAXCONN) < 0)
    {
        std::cerr << "listen fail" << std::endl;
        close(listen_fd);
        return 1;
    }

    if (set_nonblocking(listen_fd) < 0)
    {
        std::cerr << "failed to set non-blocking" << std::endl;
        close(listen_fd);
        return 1;
    }

    _listen_fd = listen_fd;
    return 0;
}

// Run the main poll() event loop on the provided listening fd.
// This function does not return until the server stops; it closes
// all client fds before returning.
void Server::run_event_loop()
{
    std::vector<struct pollfd> fds;
    struct pollfd listen_pollfd;
    listen_pollfd.fd = _listen_fd;
    listen_pollfd.events = POLLIN;
    listen_pollfd.revents = 0;
    fds.push_back(listen_pollfd);

    //Client fzf();

    std::cout << "Server listening (event loop)" << std::endl;

    char tmp_buf[1024];

    while (1)
    {
        int ready = poll(&fds[0], fds.size(), -1);
        if (ready < 0)
        {
            if (errno == EINTR)
                continue;
            std::cerr << "poll fail" << std::endl;
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            while (1)
            {
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(_listen_fd, (sockaddr *)&client_addr, &client_len);
                std::cout << client_addr.sin_addr.s_addr << std::endl;
                if (client_fd < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                    if (errno == EINTR)
                        continue;
                    std::cerr << "accept fail" << std::endl;
                    break;
                }

                if (set_nonblocking(client_fd) < 0)
                {
                    close(client_fd);
                    continue;
                }

                struct pollfd client_pollfd;
                client_pollfd.fd = client_fd;
                client_pollfd.events = POLLIN;
                client_pollfd.revents = 0;
                fds.push_back(client_pollfd);
                send(client_fd, "welcome to irc\n", 15, 0);

                //clients.append("teto", "17.0.0.2")

                std::cout << "client connected fd " << client_fd << std::endl;
            }
        }

        for (size_t i = 1; i < fds.size(); ++i)
        {
            short revents = fds[i].revents;
            if (revents == 0)
                continue;

            if (revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                std::cout << "client disconnected fd " << fds[i].fd << std::endl;
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                --i;
                continue;
            }

            if (revents & POLLIN)
            {
                ssize_t n = recv(fds[i].fd, tmp_buf, sizeof(tmp_buf), 0);
                if (n <= 0)
                {
                    std::cout << "client disconnected fd " << fds[i].fd << std::endl;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    --i;
                    continue;
                }
                std::string data(tmp_buf, (size_t)n);
                ssize_t sent = 0;
                for (size_t ind = 1; ind < fds.size(); ++ind)
                {
                    if (ind == i)
                        continue;
                    sent = 0;
                    while (sent < (ssize_t)data.size()) {
                        ssize_t s = send(fds[ind].fd, data.c_str() + sent, data.size() - sent, 0);
                        if (s <= 0) break;
                        sent += s;
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < fds.size(); ++i)
        close(fds[i].fd);
}

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

/**
 * @brief get User input and handle/parse them
 */
bool Server::parse_data(char **av) {
    //port parsing
    char *end = NULL;
    this->SetPort(std::strtol(av[1], &end, 10));
    if (GetPort() == 0 || end == av[1] || *end != '\0' || GetPort() < 6665 || GetPort() > 6669)
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

/*
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

    //verify if client is in channel
    if (!ch.hasMember(clientFd))
    {
        std::string msg = ":server 442 " + channelName + " :You're not in that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    if (!ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + channelName + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // // // a decommenter lorsque la classe Client sera cree
    // // Verify if target exist and is in channel
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

    // std::string msg = ":" + getClientPrefix(clientFd) + " KICK " + channelName + " " + targetName + " :" + reason + "\r\n";
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

    //verify if client is in channel
    if (!ch.hasMember(clientFd))
    {
        std::string msg = ":server 442 " + channelName + " :You're not in that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    //verify if client is operator
    if (!ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + channelName + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // // // a decommenter lorsque la classe Client sera cree
    // // Verify if target exist
    // int targetFd = findClientByNick(targetNick);
    // if (targetFd == -1)
    // {
    //     std::string msg = ":server 401 " + targetNick + " :user does not exist\r\n";
    //     send(clientFd, msg.c_str(), msg.size(), 0);
    //     return ;
    // }

    // ch.addInvite(targetFd);
    // std::string msg = ":server 341 " + getClientNickname(clientFd) + " " + targetNick + " " + channelName + "\r\n";
    // send(clientFd, msg.c_str(), msg.size(), 0);
    // msg = ":" + getClientPrefix(clientFd) + " INVITE " + targetNick + " :" + channelName + "\r\n";
    // send(targetFd, msg.c_str(), msg.size(), 0);
}

void Server::topic(int clientFd, const std::string& channelName, const std::string& newTopic)
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

    //verify if client is in channel
    if (!ch.hasMember(clientFd))
    {
        std::string msg = ":server 442 " + channelName + " :You're not in that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // // // a decommenter une fois la class client prete
    // //if newTopic empty, display actual topic if there is one
    // if (newTopic.empty())
    // {
    //     if (ch.getTopic().empty())
    //     {
    //         std::string msg = ":server 331 " + getClientNickname(clientFd) + " " + channelName + " :No topic is set\r\n";
    //         send(clientFd, msg.c_str(), msg.size(), 0);
    //     }
    //     else
    //     {
    //         std::string msg = ":server 332 " + getClientNickname(clientFd) + " " + channelName + ":" + ch.getTopic() + "\r\n";
    //         send(clientFd, msg.c_str(), msg.size(), 0);
    //     }
    //     return ;
    // }

    // //verify if client is operator and topic restricted
    // if (ch.isTopicRestricted() && !ch.isOperator(clientFd))
    // {
    //     std::string msg = ":server 482 " + channelName + " :You're not channel operator\r\n";
    //     send(clientFd, msg.c_str(), msg.size(), 0);
    //     return ;
    // }

    // ch.setTopic(newTopic);
    // std::string msg = ":" + getClientPrefix(clientFd) + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    // ch.broadcast(msg);
}*/