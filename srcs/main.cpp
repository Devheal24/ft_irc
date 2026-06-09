/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhubier <jhubier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 10:49:01 by jhubier           #+#    #+#             */
/*   Updated: 2026/06/09 13:09:08 by jhubier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

int main (int ac, char **av)
{
    if (ac != 3) {std::cerr << "Error\n -> expected ./irc <port> <password>" << std::endl; return 1;}
    
    //parsing todo
    Server serv;
    if (!serv.parse_data(av)) {return 1;};
    if (serv.init_server() == 1) {return 1;};
    return 0;
}