/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarnier <mgarnier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:39:59 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/09 15:05:59 by mgarnier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(std::string name, in_addr_t IP)
{
    _info[name] = IP;
    return;
}

Client::~Client()
{
    return;
}

Client::Client(const Client &base)
{
    *this = base;
    return;
}

Client &Client::operator=(const Client &base)
{
    if (this != &base)
    {
        this->_info = base._info;
    }
    return (*this);
}
