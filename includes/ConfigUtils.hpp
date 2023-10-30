/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorompa <akorompa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:03:13 by akorompa          #+#    #+#             */
/*   Updated: 2023/10/30 15:27:40 by akorompa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGUTILS_HPP
# define CONFIGUTILS_HPP

#include "Config.hpp"


class ServerConf
{
    public:
        std::map<std::string, std::vector<std::string> > server_data;
        std::vector<std::map<std::string, std::map<std::string, std::vector<std::string> > > > locations;
};

class Utils
{
    public:
        std::string value;
        std::string key;
        std::string line;
        int index;
};


class FileOpenException : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Error while openening file\n";
    }
};

#endif