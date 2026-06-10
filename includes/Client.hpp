/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:37:27 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/10 12:43:00 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

/**
 * @include <string> : stockage des noms et opérations sur chaînes
 * @include <set> : structure pour membres/operateurs/invitations sans doublons
 */
#include <string>
#include <set>

class Client
{
    private:
        std::string _name;
        int         _fd;
        std::set<std::string> _joinedChannels;
        std::string           _activeChannel;
    
    public:
        Client();
        Client(std::string name, int fd);
        ~Client();
        Client(const Client &base);
        Client &operator=(const Client &base);

        void addClient(std::string name, int fd);

        // channel management
        void joinChannel(const std::string& channelName);
        void partChannel(const std::string& channelName);
        void setActiveChannel(const std::string& channelName);

        std::string getActiveChannel() const;
        bool isInChannel(const std::string& channelName) const;
        const std::set<std::string>& getJoinedChannels() const;

        // getters
        std::string getName() const;
        int getFD() const;
};

#endif