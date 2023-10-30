/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorompa <akorompa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 15:12:14 by akorompa          #+#    #+#             */
/*   Updated: 2023/10/30 15:27:36 by akorompa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <fstream>
#include <stack>
#include <map>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "Config.hpp"
#include "ConfigUtils.hpp"
#include "tools.hpp"

# define WHITE_SPACE " \t"
# define NUM "0123456789"
# define IP_NUM "0123456789:."
# define RED "\x1b[31m"
# define END_CLR "\033[0m"
# define SIZE_TYPE "MKGBmkgb"
# define ERROR_PATH "./error.html"


class Data
{
    public:
        Data() {}
        Config config;
        Serv_list server_list;
        std::string error;
};

#endif