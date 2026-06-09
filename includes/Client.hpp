/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarnier <mgarnier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:37:27 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/09 18:17:56 by mgarnier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

class Client
{
    private:
        std::vector< std::string > _name;
        std::vector< int > _fd;
    
    public:
        Client();
        ~Client();
        Client(const Client &base);
        Client &operator=(const Client &base);

        void addClient(std::string name, int fd);

        // getters
        std::string getName(int fd) const;
        int getFD(std::string name) const;
};
