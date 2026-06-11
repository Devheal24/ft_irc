#include "../includes/Server.hpp"
#include <sys/socket.h>
#include <sstream>
#include <iostream>

void Server::CommandPass(std::istringstream &iss, size_t selfIdx, int clientFd)
{
    std::string pass;
    iss >> pass;

    while (!pass.empty() &&
            (pass[pass.size() - 1] == '\r' || pass[pass.size() - 1] == '\n'))
        pass.resize(pass.size() - 1);

    _clients[selfIdx].setPass(pass);

    std::cout << "DEBUG PASS fd=" << clientFd << " pass=[" << pass << "]" << std::endl;
    return;
}

bool Server::CommandNick(std::istringstream &iss, size_t selfIdx, int clientFd)
{
    std::string nick;
    iss >> nick;

    while (!nick.empty() && 
            (nick[nick.size() - 1] == '\r' || nick[nick.size() - 1] == '\n'))
        nick.resize(nick.size() - 1);

    bool wasRegistered = _clients[selfIdx].isRegistered();
    _clients[selfIdx].setNick(nick);

    std::cout << "DEBUG NICK fd=" << clientFd << " nick=[" << nick << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;

    if (!wasRegistered && _clients[selfIdx].isRegistered())
    {
        bool needPass = !_pwd.empty();

        if (needPass && (!_clients[selfIdx].hasPass() || _clients[selfIdx].getPass() != _pwd))
        {
            std::ostringstream oss;
            oss << ":server 464 " << nick << " :Password incorrect\r\n";
            std::string msg = oss.str();

            send(clientFd, msg.c_str(), msg.size(), 0);
            std::cout << "DEBUG REGISTRATION FAILED fd=" << clientFd << " nick=" << nick << " (bad PASS) - disconnecting" << std::endl;
            return false; // disconnect client on failed registration
        }
        else
        {
            std::ostringstream w;
            w << ":server 001 " << nick << " :Welcome to the IRC server\r\n";
            std::string wmsg = w.str();

            send(clientFd, wmsg.c_str(), wmsg.size(), 0);
            std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << nick << std::endl;
        }
    }
    return true;
}

bool Server::CommandUser(std::istringstream &iss, size_t selfIdx, int clientFd)
{
    std::string user, mode, unused;
    iss >> user >> mode >> unused;
    std::string real;
    std::getline(iss, real);

    if (!real.empty() && real[0] == ' ')
        real.erase(0, 1);
    if (!real.empty() && real[0] == ':')
        real.erase(0, 1);

    while (!real.empty() &&
            (real[real.size() - 1] == '\r' || real[real.size() - 1] == '\n'))
        real.resize(real.size() - 1);

    bool wasRegistered = _clients[selfIdx].isRegistered();
    _clients[selfIdx].setUser(user, real);
    std::cout << "DEBUG USER fd=" << clientFd << " user=[" << user << "] real=[" << real << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;

    if (!wasRegistered && _clients[selfIdx].isRegistered())
    {
        std::string nick = _clients[selfIdx].getName();
        bool needPass = !_pwd.empty();

        if (needPass && (!_clients[selfIdx].hasPass() || _clients[selfIdx].getPass() != _pwd))
        {
            std::ostringstream oss;
            oss << ":server 464 " << nick << " :Password incorrect\r\n";
            std::string msg = oss.str();\

            send(clientFd, msg.c_str(), msg.size(), 0);
            std::cout << "DEBUG REGISTRATION FAILED fd=" << clientFd << " nick=" << nick << " (bad PASS) - disconnecting" << std::endl;
            return false; // disconnect client on failed registration
        }
        else
        {
            std::ostringstream w;
            w << ":server 001 " << nick << " :Welcome to the IRC server\r\n";
            std::string wmsg = w.str();

            send(clientFd, wmsg.c_str(), wmsg.size(), 0);
            std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << nick << std::endl;
        }
    }
    return true;
}

void Server::CommandJoin(std::istringstream &iss, int clientFd)
{
    std::string chan;
    iss >> chan;

    if (!chan.empty() && chan[0] == ':')
        chan = chan.substr(1);

    while (!chan.empty() &&
            (chan[chan.size() - 1] == '\r' || chan[chan.size() - 1] == '\n')) 
        chan.resize(chan.size() - 1);

    if (!chan.empty())
        joinChannel(clientFd, chan);
    return;
}

void Server::CommandPrivMsg(std::istringstream &iss, std::string &token, size_t selfIdx, int clientFd)
{
    std::string target;
    iss >> target;
    std::string message;
    std::getline(iss, message);

    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);
    if (!message.empty() && message[0] == ':')
        message.erase(0, 1);

    std::string nick = _clients[selfIdx].getName();
    if (nick.empty())
        nick = "client";

    if (!target.empty() && (target[0] == '#' || target[0] == '&'))
    {
        std::map<std::string, Channel>::iterator it = _channels.find(target);
        if (it == _channels.end())
        {
            std::cout << "DEBUG ROUTE missing channel target=[" << target << "] fd=" << clientFd << std::endl;
            return;
        }
        _clients[selfIdx].setActiveChannel(target);
        std::ostringstream prefixMsg;
        prefixMsg << ":" << nick << "!" << nick << "@localhost " << token << " " << target << " :" << message << "\r\n";
        std::string formatted = prefixMsg.str();
        it->second.broadcastExcept(clientFd, formatted);
        return;
    }

    bool delivered = false;
    for (size_t k = 0; k < _clients.size(); ++k)
    {
        if (_clients[k].getFD() == clientFd)
            return;
        if (_clients[k].getName() == target)
        {
            std::ostringstream prefixMsg;
            prefixMsg << ":" << nick << "!" << nick << "@localhost " << token << " " << target << " :" << message << "\r\n";
            std::string formatted = prefixMsg.str();
            send(_clients[k].getFD(), formatted.c_str(), formatted.size(), 0);
            delivered = true;
            break;
        }
    }
    if (!delivered)
    {
        std::ostringstream err;
        err << ":server 401 " << nick << " " << target << " :No such nick/channel\r\n";
        std::string msg = err.str();
        send(clientFd, msg.c_str(), msg.size(), 0);
    }
    return;
}
