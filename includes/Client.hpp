/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarnier <mgarnier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:37:27 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/09 15:02:41 by mgarnier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include <netinet/in.h>
#include <string>

class Client
{
    private:
        std::map< std::string, in_addr_t > _info;
    
    public:
        Client(std::string name, in_addr_t IP);
        ~Client();
        Client(const Client &base);
        Client &operator=(const Client &base);
};
