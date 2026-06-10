#include "../includes/Server.hpp"
/**
 * @include <iostream> : (std::cout / std::cerr)
 * @include <unistd.h> : fonctions POSIX (close, read, write, etc.)
 * @include <netinet/in.h> : structures d'adressage réseau (sockaddr_in) for bind/accept
 * @include <cstdlib> : atoi
 * @include <fcntl.h> : contrôle des fichiers (fcntl) pour set non-blocking
 * @include <poll.h> : interface poll() pour multiplexage
 * @include <vector> : conteneur pour la liste des pollfd
 * @include <cerrno> : codes d'erreur POSIX (errno) for accept() or revents
 * @include <cstring> : C manipulation mémoire/chaînes
 * @include <arpa/inet.h> : conversions d'adresses réseau (htons, inet_*) for addr_in (port
 * @include <sstream> : flux sur chaînes/string
 * @include <sys/socket.h> : API sockets (socket, bind, listen, accept, send)
 */
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdlib>
#include <fcntl.h>
#include <poll.h>
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
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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
    std::vector<struct pollfd> fds;
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
                

                struct pollfd client_pollfd;
                client_pollfd.fd = client_fd;
                client_pollfd.events = POLLIN;
                client_pollfd.revents = 0;
                fds.push_back(client_pollfd);
                // create placeholder client name so the client appears registered to hexChat
                std::ostringstream cn;
                cn << "client" << client_fd;
                std::string placeholderName = cn.str();
                size_t ci = 0;
                while (ci < _clients.size() && _clients[ci].getFD() != client_fd)
                    ++ci;
                if (ci == _clients.size()) {
                    // use empty name placeholder so NAMES shows nothing until client sets NICK
                    _clients.push_back(Client(std::string(""), client_fd));
                }
                std::cout << "client connected fd " << client_fd << std::endl;
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
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                --i;
                continue;
            }

            if (revents & POLLIN)
            {
                int clientFd = fds[i].fd;
                bool connected = handleClientInput(clientFd);
                if (!connected)
                {
                    std::cout << "client disconnected fd " << clientFd << std::endl;
                    removeClient(clientFd);
                    close(clientFd);
                    fds.erase(fds.begin() + i);
                    --i;
                }
            }
        }
    }

    for (size_t i = 0; i < fds.size(); ++i)
        close(fds[i].fd);
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
    if (selfIdx == _clients.size()) {
        _clients.push_back(Client(std::string(""), clientFd));
        selfIdx = _clients.size() - 1;
    }

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

        if (token == "PASS") {
            std::string pass;
            iss >> pass;
            while (!pass.empty() && (pass[pass.size() - 1] == '\r' || pass[pass.size() - 1] == '\n')) pass.resize(pass.size() - 1);
            _clients[selfIdx].setPass(pass);
            std::cout << "DEBUG PASS fd=" << clientFd << " pass=[" << pass << "]" << std::endl;
            continue;
        }

        if (token == "NICK") {
            std::string nick;
            iss >> nick;
            while (!nick.empty() && (nick[nick.size() - 1] == '\r' || nick[nick.size() - 1] == '\n')) nick.resize(nick.size() - 1);
            bool wasRegistered = _clients[selfIdx].isRegistered();
            _clients[selfIdx].setNick(nick);
            std::cout << "DEBUG NICK fd=" << clientFd << " nick=[" << nick << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;
            if (!wasRegistered && _clients[selfIdx].isRegistered()) {
                bool needPass = !_pwd.empty();
                if (needPass && (!_clients[selfIdx].hasPass() || _clients[selfIdx].getPass() != _pwd)) {
                    std::ostringstream oss;
                    oss << ":server 464 " << nick << " :Password incorrect\r\n";
                    std::string msg = oss.str();
                    send(clientFd, msg.c_str(), msg.size(), 0);
                    std::cout << "DEBUG REGISTRATION FAILED fd=" << clientFd << " nick=" << nick << " (bad PASS) - disconnecting" << std::endl;
                    return false; // disconnect client on failed registration
                } else {
                    std::ostringstream w;
                    w << ":server 001 " << nick << " :Welcome to the IRC server\r\n";
                    std::string wmsg = w.str();
                    send(clientFd, wmsg.c_str(), wmsg.size(), 0);
                    std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << nick << std::endl;
                }
            }
            continue;
        }

        if (token == "USER") {
            std::string user, mode, unused;
            iss >> user >> mode >> unused;
            std::string real;
            std::getline(iss, real);
            if (!real.empty() && real[0] == ' ') real.erase(0, 1);
            if (!real.empty() && real[0] == ':') real.erase(0, 1);
            while (!real.empty() && (real[real.size() - 1] == '\r' || real[real.size() - 1] == '\n')) real.resize(real.size() - 1);
            bool wasRegistered = _clients[selfIdx].isRegistered();
            _clients[selfIdx].setUser(user, real);
            std::cout << "DEBUG USER fd=" << clientFd << " user=[" << user << "] real=[" << real << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;
            if (!wasRegistered && _clients[selfIdx].isRegistered()) {
                std::string nick = _clients[selfIdx].getName();
                bool needPass = !_pwd.empty();
                if (needPass && (!_clients[selfIdx].hasPass() || _clients[selfIdx].getPass() != _pwd)) {
                    std::ostringstream oss;
                    oss << ":server 464 " << nick << " :Password incorrect\r\n";
                    std::string msg = oss.str();
                    send(clientFd, msg.c_str(), msg.size(), 0);
                    std::cout << "DEBUG REGISTRATION FAILED fd=" << clientFd << " nick=" << nick << " (bad PASS) - disconnecting" << std::endl;
                    return false; // disconnect client on failed registration
                } else {
                    std::ostringstream w;
                    w << ":server 001 " << nick << " :Welcome to the IRC server\r\n";
                    std::string wmsg = w.str();
                    send(clientFd, wmsg.c_str(), wmsg.size(), 0);
                    std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << nick << std::endl;
                }
            }
            continue;
        }

        // JOIN
        if (token == "JOIN" || token == "/JOIN") {
            std::string chan;
            iss >> chan;
            if (!chan.empty() && chan[0] == ':') chan = chan.substr(1);
            while (!chan.empty() && (chan[chan.size() - 1] == '\r' || chan[chan.size() - 1] == '\n')) chan.resize(chan.size() - 1);
            if (!chan.empty()) {
                joinChannel(clientFd, chan);
            }
            continue;
        }

        // PRIVMSG
        if (token == "PRIVMSG") {
            std::string target;
            iss >> target;
            std::string message;
            std::getline(iss, message);
            if (!message.empty() && message[0] == ' ') message.erase(0, 1);
            if (!message.empty() && message[0] == ':') message.erase(0, 1);
            std::string active = _clients[selfIdx].getActiveChannel();
            if (active.empty()) {
                std::string msg = "No channel joined. Try /join #<channel>\r\n";
                send(clientFd, msg.c_str(), msg.size(), 0);
                continue;
            }
            std::map<std::string, Channel>::iterator it = _channels.find(active);
            if (it == _channels.end()) continue;
            std::string nick = _clients[selfIdx].getName();
            if (nick.empty()) nick = "client";
            std::ostringstream prefixMsg;
            prefixMsg << ":" << nick << "!" << nick << "@localhost PRIVMSG " << active << " :" << message << "\r\n";
            std::string formatted = prefixMsg.str();
            it->second.broadcastExcept(clientFd, formatted);
            continue;
        }

        // fallback: deliver raw line to active channel
        if (selfIdx < _clients.size()) {
            std::string active = _clients[selfIdx].getActiveChannel();
            if (!active.empty()) {
                std::map<std::string, Channel>::iterator it = _channels.find(active);
                if (it != _channels.end()) {
                    std::string sendline = line;
                    if (sendline.size() < 2 || sendline[sendline.size()-2] != '\r') sendline += "\r\n";
                    it->second.broadcastExcept(clientFd, sendline);
                }
            } else {
                std::string msg = "You are not in any channel\r\n";
                send(clientFd, msg.c_str(), msg.size(), 0);
            }
        }
    }
    
    return true;
}

/**
 * @brief create and/or join channel
 */
void Server::joinChannel(int clientFd, const std::string& name)
{
    std::map<std::string, Channel>::iterator it;

    std::cout << "attempting to join channel : " << name << std::endl;
    it = _channels.find(name);

    if (it == _channels.end())
    {
        _channels.insert(std::make_pair(name, Channel(name)));
        it = _channels.find(name);
        std::cout << "Channel created: " << name << std::endl;
    }
    
    // if client already member, just set active channel
    if (it->second.hasMember(clientFd))
    {
        size_t j = 0;
        while (j < _clients.size() && _clients[j].getFD() != clientFd)
            ++j;
        if (j == _clients.size())
            _clients.push_back(Client(std::string(""), clientFd));
        _clients[j].setActiveChannel(name);
        std::ostringstream oss;
        oss << ":server NOTICE " << clientFd << " :Now active in " << name << "\r\n";
        std::string msg = oss.str();
        send(clientFd, msg.c_str(), msg.size(), 0);
        std::cout << "Client " << clientFd << " set active " << name << std::endl;
        return;
    }

    //firstMember is def operator
    bool firstMember = (it->second.memberCount() == 0);
    it->second.addMember(clientFd);
    if (firstMember)
    {
        it->second.addOperator(clientFd);
        std::cout << "Client " << clientFd << " is operator" << std::endl;
    }
    std::cout << "Client " << clientFd << " joined " << name << std::endl;

    size_t j = 0;
    while (j < _clients.size() && _clients[j].getFD() != clientFd)
        ++j;
    if (j == _clients.size())
        _clients.push_back(Client(std::string(""), clientFd));
    _clients[j].joinChannel(name);

    // IRC standard: broadcast JOIN message to all channel members (including sender)
    std::ostringstream oss;
    oss << ":" << _clients[j].getName() << " JOIN " << name << "\r\n";
    std::string joinMsg = oss.str();
    it->second.broadcast(joinMsg);

    // Send topic (332) or no topic (331) to the joining client
    std::string nick = _clients[j].getName();
    if (nick.empty()) nick = "*";
    if (!it->second.getTopic().empty()) {
        std::ostringstream tss;
        tss << ":server 332 " << nick << " " << name << " :" << it->second.getTopic() << "\r\n";
        std::string tmsg = tss.str();
        send(clientFd, tmsg.c_str(), tmsg.size(), 0);
    } else {
        std::ostringstream tss;
        tss << ":server 331 " << nick << " " << name << " :No topic is set\r\n";
        std::string tmsg = tss.str();
        send(clientFd, tmsg.c_str(), tmsg.size(), 0);
    }

    // Send NAMES reply (353) and end of names (366)
    std::ostringstream names;
    for (size_t kk = 0; kk < _clients.size(); ++kk) {
        int memberFd = _clients[kk].getFD();
        if (!it->second.hasMember(memberFd))
            continue;
        std::string mname = _clients[kk].getName();
        if (mname.empty()) mname = "*";
        std::cout << "DEBUG NAMES member fd=" << memberFd << " name=[" << _clients[kk].getName() << "] usedName=[" << mname << "]" << std::endl;
        names << mname;
        // detect if more members exist after kk
        bool more = false;
        for (size_t kk2 = kk + 1; kk2 < _clients.size(); ++kk2) {
            if (it->second.hasMember(_clients[kk2].getFD())) { more = true; break; }
        }
        if (more) names << ' ';
    }
    std::ostringstream r353;
    r353 << ":server 353 " << nick << " = " << name << " :" << names.str() << "\r\n";
    std::string r353s = r353.str();
    send(clientFd, r353s.c_str(), r353s.size(), 0);

    /*std::ostringstream r001;
    r001 << ":server 001 " << ":welcome" << "\r\n";
    std::string r001s = r001.str();
    send(clientFd, r001s.c_str(), r001s.size(), 0);*/

    std::ostringstream r366;
    r366 << ":server 366 " << nick << " " << name << " :End of /NAMES list\r\n";
    std::string r366s = r366.str();
    send(clientFd, r366s.c_str(), r366s.size(), 0);
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

    const std::set<std::string>& chans = _clients[j].getJoinedChannels();
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
    }

    _clients.erase(_clients.begin() + j);
}

/*
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