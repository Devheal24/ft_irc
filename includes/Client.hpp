/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgarnier <mgarnier@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 14:37:27 by mgarnier          #+#    #+#             */
/*   Updated: 2026/06/09 23:01:40 by mgarnier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

class Client
{
    private:
        std::string _name;
        int         _fd;
    
    public:
        Client();
        Client(std::string name, int fd);
        ~Client();
        Client(const Client &base);
        Client &operator=(const Client &base);

        void addClient(std::string name, int fd);

        // getters
        std::string getName() const;
        int getFD() const;
};
