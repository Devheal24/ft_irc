#include "../includes/Server.hpp"
#include <sys/socket.h>
#include <sstream>
#include <iostream>

/**
 * @brief handler for PASS flag recv() from hexchat.
 * PASS -> (password input by user)
 */
void Server::CommandPass(std::istringstream &iss, size_t selfIdx, int clientFd)
{
    std::string pass;
    iss >> pass;

    while (!pass.empty() &&
            (pass[pass.size() - 1] == '\r' || pass[pass.size() - 1] == '\n'))
        pass.resize(pass.size() - 1);

    if (pass.length() < 8 || pass.length() > 64)
    {
        std::cout << "invalid length of pass (64> || <8)" << pass << std::endl;
        return;
    }
    for (size_t i = 0; i < pass.length(); i++)
    {
        if (!isprint(pass[i]) && pass[i] != '_' && pass[i] != '-')
        {
            std::cout << "invalid character in passname" << pass << std::endl;
            return;
        }
    }
    

    /*if (pass != _pwd)
    {
        std::cout << "passeword doesnt match server pwd" << pass << std::endl;
        return;
    }*/

    _clients[selfIdx].setPass(pass);

    std::cout << "DEBUG PASS fd=" << clientFd << " pass=[" << pass << "]" << std::endl;
    return;
}

/**
 * @brief handler for NICK flag recv() from hexchat.
 * NICK -> (nickname input by user)
 * -- also check pwd and name validity here --
 */
bool Server::CommandNick(std::istringstream &iss, size_t selfIdx, int clientFd)
{
    std::string nick;
    iss >> nick;

    while (!nick.empty() && 
            (nick[nick.size() - 1] == '\r' || nick[nick.size() - 1] == '\n'))
        nick.resize(nick.size() - 1);


    //test norm
    if (nick.length() >= 10)
    {
        std::cout << "invalid length of nickname (>10)" << nick << std::endl;
        return (true);
    }
    for (size_t i = 0; i < nick.length(); i++)
    {
        if (!isdigit(nick[i]) && !isalnum(nick[i]) && nick[i] != '_' && nick[i] != '-')
        {
            std::cout << "invalid character in nickname" << nick << std::endl;
            return (true);
        }
    }
    
    bool wasRegistered = _clients[selfIdx].isRegistered();
    _clients[selfIdx].setNick(nick);

    size_t j = -1;
    while (++j < _clients.size())
    {
        if (_clients[j].getName() == nick && _clients[j].getFD() != clientFd)
        {
            std::cerr << "nickname already used !" << std::endl;
            std::string msg = ":server 433 : " + nick + " :Nickname is already in use\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
            return false;
        }
    }

    std::cout << "DEBUG NICK fd=" << clientFd << " nick=[" << nick << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;

    std::cout << !wasRegistered << " + " << _clients[selfIdx].isRegistered() << std::endl;
    if (!wasRegistered && _clients[selfIdx].isRegistered())
    {
        bool needPass = !_pwd.empty();
        bool hasPass = !_clients[selfIdx].getPass().empty();
        if (needPass != hasPass || _clients[selfIdx].getPass() != _pwd)
        {
            std::string msg = numRep(464, nick);
            send(clientFd, msg.c_str(), msg.size(), 0);
            std::cout << "DEBUG REGISTRATION FAILED fd=" << clientFd << " nick=" << nick << " (bad PASS) - disconnecting" << std::endl;
            return false; // disconnect client on failed registration
        }
        else
        {
            std::string wmsg = numRep(001, nick);
            send(clientFd, wmsg.c_str(), wmsg.size(), 0);
            std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << nick << std::endl;
        }
    }
    return true;
}

/**
 * @brief handler for USER flag recv() from hexchat.
 * USER -> (username input by user)
 */
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

    
    //test norm
    if (user.length() >= 20)
    {
        std::cout << "invalid length of username (>20)" << user << std::endl;
        return (true);
    }
    for (size_t i = 0; i < user.length(); i++)
    {
        if (!isdigit(user[i]) && !isalnum(user[i]) && user[i] != '_' && user[i] != '-')
        {
            std::cout << "invalid character in username" << user << std::endl;
            return (true);
        }
    }

    bool wasRegistered = _clients[selfIdx].isRegistered();
    _clients[selfIdx].setUser(user, real);

    size_t j = -1;
    while (++j < _clients.size())
    {
        if (_clients[j].getUsername() == user && _clients[j].getFD() != clientFd)
        {
            std::cerr << "Username already used !" << std::endl; return false;
        }
    }

    std::cout << "DEBUG USER fd=" << clientFd << " user=[" << user << "] real=[" << real << "] registered=" << _clients[selfIdx].isRegistered() << std::endl;

    if (!wasRegistered && _clients[selfIdx].isRegistered())
    {
        std::string nick = _clients[selfIdx].getName();
        bool needPass = !_pwd.empty();
        bool hasPass = !_clients[selfIdx].getPass().empty();

        if (needPass != hasPass || _clients[selfIdx].getPass() != _pwd)
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
            w << ":server 001 " << nick << " :Welcome to the IRC server, " << nick << "\r\n";
            std::string wmsg = w.str();

            send(clientFd, wmsg.c_str(), wmsg.size(), 0);
            std::cout << "DEBUG REGISTERED fd=" << clientFd << " nick=" << nick << std::endl;
        }
    }
    return true;
}

/**
 * @brief handler for any msg input recv() from hexchat.
 * PRIVMSG -> any input to wich user can send an auto-reply msg
 * NOTICE -> any input to wich user can't send an auto-reply msg
 */
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
        prefixMsg << ":" << nick << "!" << _clients[selfIdx].getUsername() << "@localhost " << token << " " << target << " :" << message << "\r\n";
        std::string formatted = prefixMsg.str();
        it->second.broadcastExcept(clientFd, formatted);
        return;
    }

    bool delivered = false;
    for (size_t k = 0; k < _clients.size(); ++k)
    {
        if (_clients[k].getFD() == clientFd)
            continue;
        if (_clients[k].getName() == target)
        {
            std::ostringstream prefixMsg;
            prefixMsg << ":" << nick << "!" << _clients[selfIdx].getUsername() << "@localhost " << token << " " << target << " :" << message << "\r\n";
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

/**
 * @brief handler for JOIN cmd recv() from hexchat.
 * JOIN -> let you join or set as active a new channel
 */
void Server::CommandJoin(std::istringstream &iss, int clientFd)
{
    Client* client = getClientByFd(clientFd);
    std::string chan;
    std::string key;
    iss >> chan;
    iss >> key;

    if (!chan.empty() && chan[0] == ':')
        chan = chan.substr(1);

    while (!chan.empty() &&
            (chan[chan.size() - 1] == '\r' || chan[chan.size() - 1] == '\n')) 
        chan.resize(chan.size() - 1);

    if (!chan.empty() && chan[0] == '#')
        joinChannel(clientFd, chan, key);
    else
    {
        std::ostringstream err;
        err << ":server 403 " << client->getName() << " " << chan << " :No such channel\r\n";
        std::string msg = err.str();
        send(clientFd, msg.c_str(), msg.size(), 0);
    }
    return;
}

void Server::joinChannel(int clientFd, const std::string& name, const std::string& key)
{
    std::map<std::string, Channel>::iterator it;
    Client* client = getClientByFd(clientFd);
    std::string clientName = client->getName();

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
        _clients[j].setActiveChannel(name);
        std::ostringstream oss;
        oss << ":server NOTICE " << clientFd << " :Now active in " << name << "\r\n";
        std::string msg = oss.str();
        send(clientFd, msg.c_str(), msg.size(), 0);
        std::cout << "Client " << clientFd << " set active " << name << std::endl;
        return;
    }

	// Check if channel is InviteOnly
	if (it->second.isInviteOnly() == true)
	{
		if (!it->second.isInvited(clientFd))
		{
			std::string msg = ":server 473 " + clientName + " " + name + " :Cannot join, channel is in invite only (+i)\r\n";
			return ;
		}
	}

    // Check if channel has key and if key is matching
    if (it->second.hasKey())
    {
        if (key != it->second.getKey())
        {
            std::string msg = ":server 475 " + clientName + " " + name + " :Cannot join channel, bad password (+k)\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
            return ;
        }
    }

    // Check if channel is full with limit enable
    if (it->second.isFull())
    {
        std::string msg = ":server 471 " + clientName + " " + name + " :Cannot join, channel is full (+l)\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // firstMember is def operator
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
    _clients[j].joinChannel(name);


    /**
     * @brief all rnf data that we send to hexchat to give context on the connexion
     */
    // IRC standard: broadcast JOIN message to all channel members (including sender)
    std::ostringstream oss;
    oss << ":" << _clients[j].getName() << " JOIN " << name << "\r\n";
    std::string joinMsg = oss.str();
    it->second.broadcast(joinMsg);

    // Send topic (332) or no topic (331) to the joining client
    if (clientName.empty()) clientName = "*";
    if (!it->second.getTopic().empty()) {
        std::ostringstream tss;
        tss << ":server 332 " << clientName << " " << name << " :" << it->second.getTopic() << "\r\n";
        std::string tmsg = tss.str();
        send(clientFd, tmsg.c_str(), tmsg.size(), 0);
    } else {
        std::ostringstream tss;
        tss << ":server 331 " << clientName << " " << name << " :No topic is set\r\n";
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
        if (it->second.isOperator(memberFd))
            mname = "@" + mname;
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
    r353 << ":server 353 " << clientName << " = " << name << " :" << names.str() << "\r\n";
    std::string r353s = r353.str();
    send(clientFd, r353s.c_str(), r353s.size(), 0);

    std::ostringstream r366;
    r366 << ":server 366 " << clientName << " " << name << " :End of /NAMES list\r\n";
    std::string r366s = r366.str();
    send(clientFd, r366s.c_str(), r366s.size(), 0);
}

void Server::CommandKick(std::istringstream &iss, int clientFd)
{
    std::string channel;
    std::string target;
    std::string reason;

    iss >> channel;
    iss >> target;
    std::getline(iss, reason);
    if (!reason.empty() && reason[0] == ' ')
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);
    kick(clientFd, channel, target, reason);
}

void Server::CommandInvite(std::istringstream &iss, int clientFd)
{
    std::string target;
    std::string channel;

    iss >> target;
    iss >> channel;
    invite(clientFd, target, channel);
}

void Server::CommandTopic(std::istringstream &iss, int clientFd)
{
    std::string channel;
    std::string NewTopic;

    iss >> channel;
    iss >> NewTopic;
    topic(clientFd, channel, NewTopic);
}

void Server::CommandClose(std::istringstream &iss, int clientFd)
{
    /*std::string tmp;
    iss >> tmp;
    if (!tmp.empty())
    {
        std::cout << "close syntaxe error" << std::endl;
        return;
    }*/
    (void)iss;

    std::cerr << "close cmd used " << std::endl;

    size_t j = 0;
    while (j < _clients.size() && _clients[j].getFD() != clientFd)
        ++j;
    if (j == _clients.size())
        return;

    const std::string& chans = _clients[j].getActiveChannel();
    std::map<std::string, Channel>::iterator cit = _channels.find(chans);
    cit->second.removeMember(clientFd);
    cit->second.removeOperator(clientFd);
    if (cit->second.memberCount() == 0)
    {
        _channels.erase(cit);
        std::cout << "channel erase (0 member)"  << std::endl;
    }

    //send
}

void Server::CommandMode(std::istringstream &iss, int clientFd)
{
    Client* client = getClientByFd(clientFd);
    std::string clientName = client->getName();
    std::string channel;
    std::string mode;

    iss >> channel;
    iss >> mode;

    //verify if channel exist
    std::map<std::string, Channel>::iterator it = _channels.find(channel);
    if (it == _channels.end())
    {
        std::string msg = ":server 403 " + clientName + " " + channel + " :No such channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }
    Channel& ch = it->second;

    //verify is client is operator
    if (!ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + clientName + " " + channel + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    bool sign = (mode[0] == '+');
    if (mode.size() != 2)
    {
        std::string msg = ":server 472 " + clientName + " " + mode + " :is unknown mode char\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }
    for (size_t i = 1; i < mode.size(); i++)
    {
        switch (mode[i])
        {
        case 'i':
            ch.setInviteOnly(sign);
            ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");
            break;
        case 't':
            ch.setTopicRestricted(sign);
            ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");
            break;
        case 'k':
        {
            std::string key;
            iss >> key;
            if (key.empty())
            {
                std::string msg = ":server 461 " + clientName + " :Not enough parameters\r\n";
                send(clientFd, msg.c_str(), msg.size(), 0);
                return ;
            }
            if (sign == true)
            {
                ch.setKey(key);
                ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + " " + key + "\r\n");
            }
            else
                ch.removeKey();
            ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");
            break;
        }
        case 'o':
        {
            std::string targetName;
            iss >> targetName;

            int targetFd = getClientFdByName(targetName);
            if (targetFd == -1)
            {
                std::string msg = ":server 401 " + clientName + " " + targetName + " :No such nick\r\n";
                send(clientFd, msg.c_str(), msg.size(), 0);
                return ;
            }
            if (sign == true)
                ch.addOperator(targetFd);
            else
                ch.removeOperator(targetFd);
            ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + " " + targetName + "\r\n");
            break;
        }
        case 'l':
        {
            if (sign == true)
            {
                int limit = 0;
                iss >> limit;
                if (limit <= 0)
                    return ;
                ch.setLimit(static_cast<size_t>(limit));
                std::ostringstream oss;
                oss << ":" << getClientPrefix(clientFd) << " MODE " << channel << " " << mode << " " << limit << "\r\n";
                std::string s = oss.str();
                ch.broadcast(s);
            }
            else
            {
                ch.removeLimit();
                ch.broadcast(":" + getClientPrefix(clientFd) + " MODE " + channel + " " + mode + "\r\n");
            }
            break;
        }
        default:
            std::string msg = ":server 472 " + clientName + " " + mode + " :is unknown mode char\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
            break;
        }
    }
}

void Server::kick(int clientFd, const std::string& channelName, const std::string& targetName, const std::string& reason)
{
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    Client* client = getClientByFd(clientFd);
    std::string clientName = client->getName();

    //verify if channel exist
    if (it == _channels.end())
    {
        std::string msg = ":server 403 " + clientName + " " + channelName + " :No such channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    Channel& ch = it->second;

    //verify if client is in channel
    if (!ch.hasMember(clientFd))
    {
        std::string msg = ":server 442 " + clientName + " " + channelName + " :You're not on that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    //verify if client is operator
    if (!ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + clientName + " " + channelName + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // Verify if target exist and is in channel
    std::map<int, Client> _clients;

    int targetFd = getClientFdByName(targetName);
    if (!ch.hasMember(targetFd))
    {
        std::string msg = ":server 441 " + clientName + " " + targetName + " :No such nick on that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    ch.printMembers();
    std::string msg = ":" + getClientPrefix(clientFd) + " KICK " + channelName + " " + targetName + " :" + reason + "\r\n";
    ch.broadcast(msg);

    ch.removeMember(targetFd);
    ch.removeOperator(targetFd);
}

void Server::invite(int clientFd, const std::string& targetNick, const std::string& channelName)
{
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    Client* client = getClientByFd(clientFd);
    std::string clientName = client->getName();

    //verify if channel exist
    if (it == _channels.end())
    {
        std::string msg = ":server 403 " + clientName + " " + channelName + " :No such channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    Channel& ch = it->second;

    //verify if client is in channel
    if (!ch.hasMember(clientFd))
    {
        std::string msg = ":server 442 " + clientName + " " + channelName + " :You're not in that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    //verify if client is operator
    if (!ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + clientName + " " + channelName + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // Verify if target exist
    int targetFd = getClientFdByName(targetNick);
    if (targetFd == -1)
    {
        std::string msg = ":server 441 " + clientName + " " + targetNick + " :No such nick on that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    // Verify if target is already in channel
    if (ch.hasMember(targetFd))
    {
        std::string msg = ":server 443 " + clientName + " " + targetNick + " :Is already on channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    ch.addInvite(targetFd);
    std::string msg = ":server 341 " + clientName + " " + targetNick + " " + channelName + "\r\n";
    send(clientFd, msg.c_str(), msg.size(), 0);
    msg = ":" + getClientPrefix(clientFd) + " INVITE " + targetNick + " :" + channelName + "\r\n";
    send(targetFd, msg.c_str(), msg.size(), 0);
}

void Server::topic(int clientFd, const std::string& channelName, std::string& newTopic)
{
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    Client* client = getClientByFd(clientFd);
    std::string clientName = client->getName();

    //verify if channel exist
    if (it == _channels.end())
    {
        std::string msg = ":server 403 " + clientName + " " + channelName + " :No such channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    Channel& ch = it->second;

    //verify if client is in channel
    if (!ch.hasMember(clientFd))
    {
        std::string msg = ":server 442 " + clientName + " " + channelName + " :You're not in that channel\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    //if newTopic empty, display actual topic if there is one
    if (newTopic.empty())
    {
        if (ch.getTopic().empty())
        {
            std::string msg = ":server 331 " + clientName + " " + channelName + " :No topic is set\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
        }
        else
        {
            std::string msg = ":server 332 " + clientName + " " + channelName + " :" + ch.getTopic() + "\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
        }
        return ;
    }

    //verify if client is operator and topic restricted
    if (ch.isTopicRestricted() && !ch.isOperator(clientFd))
    {
        std::string msg = ":server 482 " + clientName + " " + channelName + " :You're not channel operator\r\n";
        send(clientFd, msg.c_str(), msg.size(), 0);
        return ;
    }

    if (newTopic[0] == ' ')
        newTopic.erase(0, 1);
    if (newTopic[0] == ':')
        newTopic.erase(0, 1);
    ch.setTopic(newTopic);
    std::string msg = ":" + getClientPrefix(clientFd) + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    ch.broadcast(msg);
}