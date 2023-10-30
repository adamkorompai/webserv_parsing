/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Checker.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorompa <akorompa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:26:48 by akorompa          #+#    #+#             */
/*   Updated: 2023/10/30 15:30:03 by akorompa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Webserv.hpp"

void    Config::check_validity(Data &wbsv_data)
{
    if (!wbsv_data.error.length())
    {
        server_data(wbsv_data);
        server_location(wbsv_data);
        set_default(wbsv_data);
    }
}

void    Config::server_data(Data &wbsv_data)
{
    Serv_list &server_list = wbsv_data.server_list;

    for(Serv_list::iterator it = server_list.begin(); it != server_list.end() && !wbsv_data.error.length(); ++it)
    {
        for (map_vector_it server_data_it = it->server_data.begin(); server_data_it != it->server_data.end() && !wbsv_data.error.length(); ++server_data_it)
        {
            for (std::vector<std::string>::iterator value_it = server_data_it->second.begin(); value_it != server_data_it->second.end() && !wbsv_data.error.length(); value_it++)
            {
                if (server_data_it->first == "listen")
                    valid_listen(wbsv_data, (*value_it));
                else if (server_data_it->first == "client_max_body_size")
                    valid_body(wbsv_data, server_data_it);
                else if (server_data_it->first == "root")
                    check_one_arg(wbsv_data, server_data_it);
                else if (server_data_it->first == "index")
                    check_one_arg(wbsv_data, server_data_it);           
           }
        }
    }
}

void    Config::valid_listen(Data &wbsv_data, std::string value)
{
    struct addrinfo hints, *res = NULL;
    std::string host, port;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    status = value.find_first_not_of(IP_NUM);
    if (status != -1)
    {
        wbsv_data.error = "Webserv: lol host not found in \"";
        wbsv_data.error += value + "\" of the \"listen\" directive in " + this->filename;
    }
    else if ((status = value.find(":")) != -1)
    {
        host = value.substr(0, value.find(":"));
        port = value.substr(value.find(":") + 1, value.length() - 1);
        if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
        {
            wbsv_data.error = "Webserv: host not found in \"" + value + "\" of the \"listen\" directive in ";
            wbsv_data.error += this->filename;
        }
    }
    else if ((status = value.find(".")) != -1)
    {
        if (getaddrinfo(value.c_str(), NULL, &hints, &res) != 0)
        {
            wbsv_data.error = "Webserv: host not found in \"" + value + "\" of the \"listen\" directive in ";
            wbsv_data.error += this->filename;
        }
    }
    else
    {
        if (getaddrinfo(NULL, value.c_str(), &hints, &res) != 0)
        {
            wbsv_data.error = "Webserv: host not found in \"" + value + "\" of the \"listen\" directive in ";
            wbsv_data.error += this->filename;
        }
    }
    freeaddrinfo(res);
}

void    Config::valid_body(Data &wbsv_data, map_vector_it server_data_it)
{
    char type;
    int index;
    std::string number;

    if (valid_vector_size(server_data_it->second, 1))
    {
        std::vector<std::string>::iterator value_it = server_data_it->second.begin();
            type = (*value_it)[(*value_it).length() - 1];
            if (is_char_in_str(SIZE_TYPE, type)) {
                number = *value_it;
                number.erase((*value_it).length() - 1, 1);
                index = number.find_first_not_of("0123456789");
                if (index != -1) {
                    wbsv_data.error = "Webserv: \"" + server_data_it->first + "\" directive invalid value in ";
                    wbsv_data.error += this->filename;
                }
                else
                    convert_body_size(*value_it, number, type);
            }
            else {
                wbsv_data.error = "Webserv: \"" + server_data_it->first + "\" directive invalid value in ";
                wbsv_data.error += this->filename;
            }
    }
    else {
        wbsv_data.error = "Webserv: invalid number of arguments in \"" + server_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
    }
}

void    Config::check_one_arg(Data &wbsv_data, map_vector_it server_data_it)
{
    if (!valid_vector_size(server_data_it->second, 1)) {
        wbsv_data.error = "Webserv: invalid number of arguments in \"" + server_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
    }
}

void    Config::server_location(Data &wbsv_data)
{
    Serv_list &server_list = wbsv_data.server_list;

    for (Serv_list::iterator it = server_list.begin(); it != server_list.end()  && !wbsv_data.error.length(); ++it) {
        for (vector_location_it locations_it = it->locations.begin(); locations_it != it->locations.end(); ++locations_it) {
            for (map_path_location_it location_path_it = locations_it->begin(); location_path_it != locations_it->end(); ++location_path_it) {
                for (map_vector_it location_data_it = location_path_it->second.begin(); location_data_it != location_path_it->second.end(); ++location_data_it) {
                    if (location_data_it->first == "allow_methods")
                        check_allow_method(wbsv_data, location_data_it);
                    else if (location_data_it->first == "root")
                        check_one_arg(wbsv_data, location_data_it);
                    else if (location_data_it->first == "return")
                        check_return_location(wbsv_data, location_data_it);
                    else if (location_data_it->first == "autoindex")
                        check_autoindex_location(wbsv_data, location_data_it);
                    else if (location_data_it->first == "index")
                        check_one_arg(wbsv_data, location_data_it);
                    else if (location_data_it->first == "upload_store")
                        check_one_arg(wbsv_data, location_data_it);
                    else if (location_data_it->first == "fastcgi_pass")
                        check_one_arg(wbsv_data, location_data_it);
                    else {
                        wbsv_data.error = "Webserv: unknown directive \"" + location_data_it->first + "\" in ";
                        wbsv_data.error += this->filename;
                    }
                }
            }
        }
    }
}

void    Config::check_allow_method(Data &wbsv_data, map_vector_it location_data_it)
{
    for (std::vector<std::string>::iterator value_it = location_data_it->second.begin(); value_it != location_data_it->second.end(); ++value_it) {
        if (*value_it == "GET" || *value_it == "POST" || *value_it == "DELETE")
            continue;
        else {
            wbsv_data.error = "Webserv: invalid arguments \"" + *value_it + "\" in allow_methods in " + this->filename;
        }
    }
}

void    Config::check_return_location(Data &wbsv_data, map_vector_it location_data_it)
{
    int index = 0;
    
    if (!valid_vector_return_size(location_data_it->second)) {
        wbsv_data.error = "Webserv: invalid number of arguments in \"" + location_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
    }
    else {
        for (std::vector<std::string>::iterator value_it = location_data_it->second.begin(); value_it != location_data_it->second.end(); ++value_it) {
            index++;
            if (index % 2) {
                if (!valid_return_status(wbsv_data, *value_it)) {
                    wbsv_data.error = "Webserv: invalid return code \"" + *value_it;
                    wbsv_data.error += "\" in " + this->filename;
                }
            }
        }
    }
}

int Config::valid_return_status(Data &wbsv_data, std::string status)
{
    int number = std::atoi(status.c_str());
    if (number >= 300 && number <= 399)
        return (1);
    wbsv_data.error = "Webserv: invalid return code \"" + status + "\" in ";
    wbsv_data.error += this->filename;
    return (0);
}

void    Config::check_autoindex_location(Data &wbsv_data, map_vector_it location_data_it)
{
    if (!valid_vector_size(location_data_it->second, 1)) {
        wbsv_data.error = "Webserv: invalid number of arguments in \"" + location_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
    }
    else {
        if (*(location_data_it->second.begin()) != "on" && *(location_data_it->second.begin()) != "off") {
            wbsv_data.error = "Webserv: invalid value \"" + *(location_data_it->second.begin()) + "\" in \"autoindex\" directive, it must be \"on\" or \"off\" in ";
            wbsv_data.error += this->filename;
        }
    }
}

void    Config::location(ServerConf &server, Data &wbsv_data, Utils utils)
{
    std::map<std::string, std::vector<std::string> >                        location_var;
    std::map<std::string,std::map<std::string, std::vector<std::string> > > location;
    std::string                                                             variable;
    std::string                                                             path;

    trim(utils.value, " \t'[]");
    if (utils.value[utils.value.length() - 1] == '{') {
        path = utils.value.substr(0, utils.value.length() - 1);
        trim(path, " \t'[]");
        utils.value = "{";
    }
    else {
        path = utils.value;
        while (std::getline(this->_infile, utils.line)) {
            this->line_index++;
            trim(utils.line, " \t'[]");
            if(utils.line.length() == 0 || utils.line[0] == '#')
                continue;
            else {
                utils.value = utils.line;
                break;
            }
        }
    }
    if (path.length() && utils.value == "{") {
        while (std::getline(this->_infile, utils.line)){
            this->line_index++;
            trim(utils.line, " \t:'[]");
            if(utils.line.length() == 0 || utils.line[0] == '#')
                continue;
            if (utils.line == "}")
                break;
            utils.index = utils.line.find(" ");
            if(utils.index == -1) {
                wbsv_data.error = "Webserv: invalida number of arguments in \"";
                wbsv_data.error += utils.line + "\" directive in " + this->filename + ':' + ft_to_string(this->line_index);
            }
            else {
                utils.key = utils.line.substr(0, utils.index);
                utils.line.erase(0, utils.index + 1);
                trim(utils.line, " \t:'[]");
                utils.value = utils.line;
                check_semicolons(utils, wbsv_data);
                trim(utils.value, " \t:'[] ;");
                utils.line = utils.value;
                while ((utils.index = utils.line.find(" ")) != -1) {
                    utils.value = utils.line.substr(0, utils.index);
                    trim(utils.value, " \t:',[]");
                    location_var[utils.key].push_back(utils.value);
                    utils.line.erase(0, utils.index + 1);
                    trim(utils.line, " \t:',[]");
                }
                trim(utils.line, " \t:',[]");
                utils.value = utils.line;
                location_var[utils.key].push_back(utils.value);
            }
        }
        location[path] = location_var;
        server.locations.push_back(location);
    }
    else {
        wbsv_data.error = "syntax error in the location bloc in";
        wbsv_data.error += this->filename + ':' + ft_to_string(this->line_index);
    }
}

bool check_exist_server_data(map_vector server_data, std::string key)
{
    if (server_data.count(key) > 0)
        return (true);
    return (false);
}

void set_default_error_vector(std::vector<std::string> &error_page)
{
    error_page.push_back("400");
    error_page.push_back(ERROR_PATH);
    error_page.push_back("500");
    error_page.push_back(ERROR_PATH);
}

void    Config::set_default(Data &wbsv_data)
{
    std::vector<std::string> vector_value;
    Serv_list &server_list = wbsv_data.server_list; 
    
    for (Serv_list::iterator it = server_list.begin(); it != server_list.end()  && !wbsv_data.error.length(); ++it) {
        if (!check_exist_server_data(it->server_data, "error_page")) {
            set_default_error_vector(vector_value);
            it->server_data.insert(std::make_pair("error_page",vector_value));
        }
        if (!check_exist_server_data(it->server_data, "listen"))
            wbsv_data.error = "Webserv: need to define listen directive in the " + this->filename;
        if (!check_exist_server_data(it->server_data, "root"))
            wbsv_data.error = "Webserv: need to define root directive in the " + this->filename;
        if (!check_exist_server_data(it->server_data, "index")) {
            vector_value.clear();
            vector_value.push_back("index.html");
            it->server_data.insert(std::make_pair("index", vector_value));
        }
        if (!check_exist_server_data(it->server_data, "client_max_body_size")) {
            vector_value.clear();
            vector_value.push_back("2147483648");
            it->server_data.insert(std::make_pair("client_max_body_size", vector_value));
        }
    }
}