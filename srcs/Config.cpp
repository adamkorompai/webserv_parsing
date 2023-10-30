/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorompa <akorompa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:17:29 by akorompa          #+#    #+#             */
/*   Updated: 2023/10/30 15:29:11 by akorompa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Webserv.hpp"

void    Config::printData(ServerConf server)
{
        for (map_vector_it server_data_it = server.server_data.begin(); server_data_it != server.server_data.end(); ++server_data_it)
        {
            std::cout << "Key: " << server_data_it->first << " Value :";
            for (std::vector<std::string>::iterator value_it = server_data_it->second.begin(); value_it != server_data_it->second.end(); value_it++)
            {
                std::cout << " " << *value_it;           
            }
            std::cout << std::endl;
        }
}

void    Config::parsing_file(std::string file, Data &wbsv_data)
{
    this->_infile.open(file.c_str());
    if (this->_infile.is_open())
    {
        this->filename = file;
        getData(wbsv_data);
    }
    else
        throw   FileOpenException();
}

void    Config::getData(Data &wbsv_data)
{
    line_index = 0;
    int index = 0;
    std::string line;
    std::string key;
    std::string value;
    Utils   utils;

    if (check_brackets(wbsv_data))
    {
        while(std::getline(this->_infile, line) && !wbsv_data.error.length())
        {
            line_index++;
            trim(line, WHITE_SPACE);
            if (line.length() == 0 || line[0] == '#')
                continue;
            index = line.find(' ');
            key = line.substr(0, index);
            if (index == -1)
            {
                std::getline(this->_infile, line);
                line_index++;
                trim(line, WHITE_SPACE);
                if (line[0] == '#')
                    continue;
                index = line.find_first_not_of(WHITE_SPACE);
                while (index == -1 && std::getline(this->_infile, line))
                {
                    line_index++;
                    trim(line, WHITE_SPACE);
                    if (line[0] == '#')
                    continue;
                    index = line.find_first_not_of(WHITE_SPACE);
                }
                value = line;
            }
            else
                value = line.substr(index, line.length() - 1);
            trim(value, WHITE_SPACE);
            utils.value = value;
            utils.key = key;
            utils.line = line;
            server_block(utils, wbsv_data);
        }
        check_validity(wbsv_data);
    }
    else {
        wbsv_data.error = "Webserv: error in syntax missing of '{' or '}' in configfile.conf";
    }
}

bool    Config::check_brackets(Data &wbsv_data)
{
    std::string line;
    int index = 0;
    std::stack<char> brace_stack;

    while(std::getline(this->_infile, line))
    {
        index++;
        trim(line, WHITE_SPACE);
        if (line[0] == '#')
            continue;
        for (unsigned int i = 0; i < line.length(); i++)
        {
            if (line[i] == '{')
                brace_stack.push(line[i]);
            else if (line[i] == '}')
            {
                if (brace_stack.empty())
                {
                    return false;
                }
                brace_stack.pop();
            }
        }        
    }
    this->_infile.clear();
    this->_infile.seekg(0, std::ios::beg);
    if (!brace_stack.empty())
    {
        wbsv_data.error = "Webserv: unexpected end of file, expecting\"}\" in ";
        wbsv_data.error += this->filename + ':' + ft_to_string(line_index);
    }
    return brace_stack.empty();
}

void    Config::server_block(Utils utils, Data &wbsv_data)
{
    ServerConf server;
    
    if (utils.key == "server" && utils.value == "{")
    {
        while (std::getline(this->_infile, utils.line) && !wbsv_data.error.length())
        {
            this->line_index++;
            trim(utils.line, WHITE_SPACE);
            if(utils.line.length() == 0 || utils.line[0] == '#')
                continue;
            if(utils.line == "}")
                break;
            utils.index = utils.line.find(' ');
            if (utils.index == -1)
            {
                wbsv_data.error = "Webserv: invalid number of arguments in \"";
                wbsv_data.error += utils.line + "\" directive in " + this->filename + ':' + ft_to_string(this->line_index);
                break;
            }
            utils.key = utils.line.substr(0, utils.index);
            utils.value = utils.line.substr(utils.index, utils.line.length());
            check_semicolons(utils, wbsv_data);
            fill_data(wbsv_data, utils, server);
        }
        if (!wbsv_data.error.length())
            wbsv_data.server_list.push_back(server);
    }
    else
        ft_error_server_block(wbsv_data, utils, this->filename, this->line_index);
}

void    Config::check_semicolons(Utils &utils, Data &wbsv_data)
{
    if (utils.key != "location" && utils.value.find_first_of(";") != (utils.value.length() - 1))
    {
        wbsv_data.error = "Webserv: directive \"";
        wbsv_data.error += utils.key + "\" is not terminated by \";\" in " + this->filename + ':' + ft_to_string(this->line_index);
    }
    else
        trim(utils.value, ";");
}

void    Config::fill_data(Data& wbsv_data, Utils utils, ServerConf &server)
{
    if (utils.key == "listen")
        key_value(server, utils);
    else if (utils.key == "server_name")
        key_value(server, utils);
    else if (utils.key == "root")
        key_value(server, utils);
    else if (utils.key == "index")
        key_value(server, utils);
    else if (utils.key == "error_page")
        key_value_error_page(server, wbsv_data, utils);
    else if (utils.key == "client_max_body_size")
        key_value(server, utils);
    else if (utils.key == "location")
        location(server, wbsv_data, utils);
    
}

void    Config::key_value(ServerConf &server, Utils utils)
{
    std::string variable;

    trim(utils.value, WHITE_SPACE);
    while ((utils.index = utils.value.find(' ')) != -1)
    {
        variable = utils.value.substr(0, utils.index);
        trim(variable, WHITE_SPACE);
        if (variable.length())
            server.server_data[utils.key].push_back(variable);
         utils.value.erase(0, utils.index + 1);
    }
    trim(utils.value, WHITE_SPACE);
    if (utils.value.length())
        server.server_data[utils.key].push_back(utils.value);
}
void    Config::key_value_error_page(ServerConf &server, Data &wbsv_data, Utils utils)
{
    std::string variable;
    int find_error = 0;

    trim(utils.value, WHITE_SPACE);
    while ((utils.index = utils.value.find(' ')) != -1)
    {
        find_error = 1;
        variable = utils.value.substr(0, utils.index);
        trim(variable, WHITE_SPACE);
        if (variable.length() && (variable.find_first_not_of(NUM) == std::string::npos) && valid_error_page(wbsv_data, variable))
            server.server_data[utils.key].push_back(variable);
        else if (!wbsv_data.error.length()){
            wbsv_data.error = "Webserv: invalid value \"" + variable + "\" in ";
            wbsv_data.error += this->filename + ':' + ft_to_string(this->line_index);
        }
        utils.value.erase(0, utils.index + 1);

    }
    trim(utils.value, " \t;");
    if (!utils.value.length() || !find_error)
    {
        wbsv_data.error = "Webserv: invalid number of arguments in \"";
        wbsv_data.error += utils.key + "\" directive in " + this->filename + ':' + ft_to_string(this->line_index);
    }
    else if (utils.value.find(".html") == std::string::npos)\
    {
        wbsv_data.error = "Webserv: " + utils.value + " is not a error file ('file.html') in \"";
        wbsv_data.error += this->filename + ':' + ft_to_string(this->line_index);
    }
    else
        server.server_data[utils.key].push_back(utils.value);
}

int     Config::valid_error_page(Data &wbsv_data, std::string error)
{
    int number = std::atoi(error.c_str());
    if (number >= 300 && number <= 599)
        return (1);
    wbsv_data.error = "Webserv: value \"" + error + "\" must be between 300 and 599 in ";
    wbsv_data.error += this->filename + ':' + ft_to_string(this->line_index);
    return (0);
}

void    Config::ft_error_server_block(Data &wbsv_data, Utils utils, std::string filename, int line_index)
{
    if (utils.key == "{"){
        wbsv_data.error = "Webserv: unexpected \"{\" in ";
        wbsv_data.error += filename + ':' + ft_to_string(line_index);
    }
    else if (utils.key != "server")
    {
        wbsv_data.error = "Webserv: unknown directive \"";
        wbsv_data.error += utils.key + "\" in " + filename + ':' + ft_to_string(line_index);
    }
    else {
        wbsv_data.error = "Webserv: directive \"server\" has no opening \"{\" in ";
        wbsv_data.error += filename + ':' + ft_to_string(line_index);
    }
}