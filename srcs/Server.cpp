/**
 * @include <iostream> : (std::cout / std::cerr)
 * @include <unistd.h> : fonctions POSIX (close, read, write, etc.)
 * @include <netinet/in.h> : structures d'adressage réseau (sockaddr_in) for bind/accept
 * @include <cstdlib> : atoi
 * @include <fcntl.h> : contrôle des fichiers (fcntl) pour set non-blocking
 * @include <vector> : conteneur pour la liste des pollfd
 * @include <cerrno> : codes d'erreur POSIX (errno) for accept() or revents
 * @include <cstring> : C manipulation mémoire/chaînes
 * @include <arpa/inet.h> : conversions d'adresses réseau (htons, inet_*) for addr_in (port
 * @include <sstream> : flux sur chaînes/string
 * @include <sys/socket.h> : API sockets (socket, bind, listen, accept, send)
 */
#include "../includes/Server.hpp"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdlib>
#include <fcntl.h>
#include <vector>
#include <cerrno>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include <sys/socket.h>

extern int g_sig;

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

/**
 * @brief Set the client to nonblocking to still loop if no new input detected
 */
static int set_nonblocking(int fd)
{
    /*int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;*/
    return fcntl(fd, F_SETFL, O_NONBLOCK);
}




/**
 * @brief Create, bind, listen and set non-blocking a listening socket for given port
 */
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

/**
 * @brief Run the main "infinite" poll() event loop on listening fd.
 */
void Server::run_event_loop()
{
    struct pollfd listen_pollfd;
    listen_pollfd.fd = _listen_fd;
    listen_pollfd.events = POLLIN;
    listen_pollfd.revents = 0;
    fds.push_back(listen_pollfd);

    std::cout << "Server listening (event loop)" << std::endl;

    while (!g_sig)
    {
        int ready = poll(&fds[0], fds.size(), -1);
        if (ready < 0)
        {
            if (errno == EINTR)
                continue;
            std::cerr << "poll fail" << std::endl;
            break;
        }

        // incoming connections
        if (fds[0].revents & POLLIN)
        {
            while (!g_sig)
            {
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(_listen_fd, (sockaddr *)&client_addr, &client_len);
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
                std::string ip = inet_ntoa(client_addr.sin_addr);
                bool client_existed = false;
                
                // check if client already exist
                for (size_t i = 0; i < _clients.size(); ++i)
                {
                    if (ip == _clients[i].getIP())
                    {
                        client_existed = true;
                        _clients[i].setFD(client_fd);
                        std::cout << "client reconnected fd " << client_fd << " with ip= " << ip << std::endl;
                        break;
                    }
                }
                if (client_existed == true)
                    continue;
                struct pollfd client_pollfd;
                client_pollfd.fd = client_fd;
                client_pollfd.events = POLLIN;
                client_pollfd.revents = 0;
                fds.push_back(client_pollfd);
                // create placeholder client name so the client appears registered to hexChat
                std::ostringstream cn;
                cn << "client" << client_fd;
                std::string placeholderName = cn.str();
                _clients.push_back(Client(std::string(""), client_fd, ip));
                std::cout << "client connected fd " << client_fd << " with ip= " << ip << std::endl;
            }
        }

        // events on client sockets
        for (size_t i = 1; i < fds.size(); ++i)
        {
            short revents = fds[i].revents;
            if (revents == 0)
                continue;

            if (revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                std::cout << "client disconnected fd " << fds[i].fd << std::endl;
                removeClient(fds[i].fd);
                /*close(fds[i].fd);
                fds.erase(fds.begin() + i)*/;
                // --i;
                continue;
            }

            // if a client have send something we handle what we received
            if (revents & POLLIN)
            {
                int clientFd = fds[i].fd;
                bool connected = handleClientInput(clientFd);
                if (!connected)
                {
                    std::cout << "client disconnected fd " << clientFd << std::endl;
                    removeClient(clientFd);
                    /*close(clientFd);
                    fds.erase(fds.begin() + i);*/
                    // --i;
                }
            }
        }
    }

    //close all fd when server shutdown
    for (size_t i = 0; i < fds.size(); ++i)
        close(fds[i].fd);

    for (size_t i = 0; i < fds.size(); ++i)
    {
        std::cout << "connected at close : " << fds[i].fd << std::endl;
    }
}

/**
 * @brief handle each client input, look for special cmd and send broadcast correctly
 */
bool Server::handleClientInput(int clientFd)
{
    char buf[1024];
    ssize_t n = recv(clientFd, buf, sizeof(buf), 0);
    if (n <= 0)
        return false;

    std::string data(buf, (size_t)n);

    // debug: raw data received
    std::cout << "DEBUG RECV fd=" << clientFd << " -> [" << data << "]" << std::endl;

    size_t selfIdx = 0;
    while (selfIdx < _clients.size() && _clients[selfIdx].getFD() != clientFd)
        ++selfIdx;

    // ensure we have a client placeholder
    // if (selfIdx == _clients.size()) {
    //     _clients.push_back(Client(std::string(""), clientFd));
    //     selfIdx = _clients.size() - 1;
    // }

    // split data into lines by LF, trim CR, and process each line
    std::vector<std::string> lines;
    std::string cur;
    for (size_t i = 0; i < data.size(); ++i) {
        char c = data[i];
        if (c == '\n') {
            if (!cur.empty() && cur[cur.size() - 1] == '\r')
                cur.resize(cur.size() - 1);
            lines.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) {
        if (cur[cur.size() - 1] == '\r')
            cur.resize(cur.size() - 1);
        lines.push_back(cur);
    }

    for (size_t li = 0; li < lines.size(); ++li) {
        std::string &line = lines[li];
        if (line.empty())
            continue;
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "PASS" || token == "/PASS")
        {
            CommandPass(iss, selfIdx, clientFd);
            continue;
        }
        if (token == "NICK" || token == "/NICK")
        {
            if (CommandNick(iss, selfIdx, clientFd) == false)
                return false;
            continue;
        }
        if (token == "USER" || token == "/USER")
        {
            if (CommandUser(iss, selfIdx, clientFd) == false)
                return false;
            continue;
        }
        if (token == "JOIN" || token == "/JOIN")
        {
            CommandJoin(iss, clientFd);
            continue;
        }
        if (token == "PRIVMSG" || token == "NOTICE")
        {
            CommandPrivMsg(iss, token, selfIdx, clientFd);
            continue;
        }
        if (token == "KICK" || token == "/KICK")
        {
            CommandKick(iss, clientFd);
            continue;   
        }
        if (token == "INVITE" || token == "/INVITE")
        {
            CommandInvite(iss, clientFd);
            continue;   
        }
        if (token == "TOPIC" || token == "/TOPIC")
        {
            CommandTopic(iss, clientFd);
            continue;   
        }
        std::cout << "DEBUG IGNORE fd=" << clientFd << " line=[" << line << "]" << std::endl;
    }
    return true;
}

/**
 * @brief when client quit irc, close/clean correctly all of his appearance in container/else
 */
void Server::removeClient(int clientFd)
{
    size_t j = 0;
    while (j < _clients.size() && _clients[j].getFD() != clientFd)
        ++j;
    if (j == _clients.size())
        return;

    /*const std::set<std::string>& chans = _clients[j].getJoinedChannels();
    for (std::set<std::string>::const_iterator it = chans.begin(); it != chans.end(); ++it)
    {
        std::map<std::string, Channel>::iterator cit = _channels.find(*it);
        if (cit != _channels.end())
        {
            cit->second.removeMember(clientFd);
            cit->second.removeOperator(clientFd);
            if (cit->second.memberCount() == 0)
                _channels.erase(cit);
        }
    }*/

    // int i = 0;
    // while (fds[i].fd != clientFd)
    //     i++;
    // close(fds[i].fd);
    // fds.erase(fds.begin() + i);
    //_clients.erase(_clients.begin() + j);
}

int Server::getClientFdByName(const std::string& name) const
{
    for (std::vector<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->getName() == name)
            return it->getFD();
    }
    return -1;
}

Client* Server::getClientByFd(int fd)
{
    for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->getFD() == fd)
            return &(*it);
    }
    return NULL;
}

const Client* Server::getClientByFd(int fd) const
{
    for (std::vector<Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->getFD() == fd)
            return &(*it);
    }
    return NULL;
}

std::string Server::getClientPrefix(int fd) const
{
    const Client* client = getClientByFd(fd);

    if (client == NULL)
        return "unknown!unknown@localhost";
    
    return client->getName() + "!" + client->getUsername() + "@localhost";
}