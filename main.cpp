#include "utils.hpp"

class Data;

class Config
{
    private:
        std::ifstream   _infile;
        std::string     filename;
        int             line_index;
    public:
        void    parsing_file(std::string file, Data &wbsv_data);
        void    getData(Data &wbsv_data);
        bool    check_brackets(Data &wbsv_data);
        void    server_block(Utils utils, Data &wbsv_data);
        void    check_semicolons(Utils &utils, Data &wbsv_data);
        void    fill_data(Data &wbsv_data, Utils utils, ServerConf &server);
        void    key_value(ServerConf &server, Utils utils);
        void    key_value_error_page(ServerConf &server, Data &wbsv_data, Utils utils);
        int     valid_error_page(Data &wbsv_data, std::string error);
        void    location(ServerConf &server, Data &wbsv_data, Utils utils);
        void    ft_error_server_block(Data &wbsv_data, Utils utils, std::string filename, int line_index);
        void    check_validity(Data &wbsv_data);
        void    server_data(Data &wbsv_data);
        void    valid_listen(Data &wbsv_data, std::string value);
        void    valid_body(Data &wbsv_data, map_vector_it server_data_it);
        void    check_one_arg(Data &wbsv_data, map_vector_it server_data_it);
        void    server_location(Data &wbsv_data);
        void    check_allow_method(Data &wbsv_data, map_vector_it location_data_it);
        void    check_return_location(Data &wbsv_data, map_vector_it location_data_it);
        int     valid_return_status(Data &g_Data, std::string status);
        void    check_autoindex_location(Data &wbsv_data, map_vector_it location_data_it);
        void    set_default(Data &wbsv_data);
};

class Data
{
    public:
        Data() {};
        Config config;
        Serv_list server_list;
        std::string error;
};


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
        wbsv_data.error = "WebServer: [emerg] error in syntax missing of '{' or '}' in configfile.conf";
    }
}

void    Config::check_validity(Data &wbsv_data)
{
    if (!wbsv_data.error.length())
    {
        server_data(wbsv_data);
        server_location(wbsv_data);
    }
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
            wbsv_data.error = "WebServer: [emerg] need to define listen directive in the " + this->filename;
        if (!check_exist_server_data(it->server_data, "root"))
            wbsv_data.error = "WebServer: [emerg] need to define root directive in the " + this->filename;
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
                        wbsv_data.error = "WebServer: [emerg] unknown directive \"" + location_data_it->first + "\" in ";
                        wbsv_data.error += this->filename;
                    }
                }
            }
        }
    }
}

void    Config::check_autoindex_location(Data &wbsv_data, map_vector_it location_data_it)
{
    if (!valid_vector_size(location_data_it->second, 1)) {
        wbsv_data.error = "WebServer: [emerg] invalid number of arguments in \"" + location_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
    }
    else {
        if (*(location_data_it->second.begin()) != "on" && *(location_data_it->second.begin()) != "off") {
            wbsv_data.error = "WebServer: [emerg] invalid value \"" + *(location_data_it->second.begin()) + "\" in \"autoindex\" directive, it must be \"on\" or \"off\" in ";
            wbsv_data.error += this->filename;
        }
    }
}

void    Config::check_return_location(Data &wbsv_data, map_vector_it location_data_it)
{
    int index = 0;
    
    if (!valid_vector_return_size(location_data_it->second)) {
        wbsv_data.error = "WebServer: [emerg] invalid number of arguments in \"" + location_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
    }
    else {
        for (std::vector<std::string>::iterator value_it = location_data_it->second.begin(); value_it != location_data_it->second.end(); ++value_it) {
            index++;
            if (index % 2) {
                if (!valid_return_status(wbsv_data, *value_it)) {
                    wbsv_data.error = "WebServer: [emerg] invalid return code \"" + *value_it;
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
    wbsv_data.error = "WebServer: [emerg] invalid return code \"" + status + "\" in ";
    wbsv_data.error += this->filename;
    return (0);
}

void    Config::check_allow_method(Data &wbsv_data, map_vector_it location_data_it)
{
    for (std::vector<std::string>::iterator value_it = location_data_it->second.begin(); value_it != location_data_it->second.end(); ++value_it) {
        if (*value_it == "GET" || *value_it == "POST" || *value_it == "DELETE")
            continue;
        else {
            wbsv_data.error = "WebServer: [emerg] invalid arguments \"" + *value_it + "\" in allow_methods in " + this->filename;
        }
    }
}

void    Config::server_data(Data &wbsv_data)
{
    Serv_list &server_list = wbsv_data.server_list;

    for(Serv_list::iterator it = server_list.begin(); it != server_list.end() && !wbsv_data.error.length(); ++it)
    {
        for (map_vector_it server_data_it = it->server_data.begin(); server_data_it != it->server_data.end() && !wbsv_data.error.length(); ++server_data_it)
        {
            for (std::vector<std::string>::iterator value_it = server_data_it->second.begin(); value_it != server_data_it->second.end() && wbsv_data.error.length(); value_it++)
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

void    Config::check_one_arg(Data &wbsv_data, map_vector_it server_data_it)
{
    if (!valid_vector_size(server_data_it->second, 1)) {
        wbsv_data.error = "WebServer: [emerg] invalid number of arguments in \"" + server_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
    }
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
                    wbsv_data.error = "WebServer: [emerg] \"" + server_data_it->first + "\" directive invalid value in ";
                    wbsv_data.error += this->filename;
                }
                else
                    convert_body_size(*value_it, number, type);
            }
            else {
                wbsv_data.error = "WebServer: [emerg] \"" + server_data_it->first + "\" directive invalid value in ";
                wbsv_data.error += this->filename;
            }
    }
    else {
        wbsv_data.error = "WebServer: [emerg] invalid number of arguments in \"" + server_data_it->first;
        wbsv_data.error += "\" in " + this->filename;
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
    if (status == -1)
    {
        wbsv_data.error = "WebServer: [emerg] host not found in \"";
        wbsv_data.error += value + "\" of the \"listen\" directive in " + this->filename;
    }
    else if ((status = value.find(":")) != -1)
    {
        host = value.substr(0, value.find(":"));
        port = value.substr(value.find(":") + 1, value.length() - 1);
        if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0)
        {
            wbsv_data.error = "WebServer: [emerg] host not found in \"" + value + "\" of the \"listen\" directive in ";
            wbsv_data.error += this->filename;
        }
    }
    else if ((status = value.find(".")) != -1)
    {
        if (getaddrinfo(value.c_str(), NULL, &hints, &res) != 0)
        {
            wbsv_data.error = "WebServer: [emerg] host not found in \"" + value + "\" of the \"listen\" directive in ";
            wbsv_data.error += this->filename;
        }
    }
    else
    {
        if (getaddrinfo(NULL, value.c_str(), &hints, &res) != 0)
        {
            wbsv_data.error = "WebServer: [emerg] host not found in \"" + value + "\" of the \"listen\" directive in ";
            wbsv_data.error += this->filename;
        }
    }
    freeaddrinfo(res);
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
                wbsv_data.error = "WebServer: [emerg] invalid number of arguments in \"";
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

void    Config::ft_error_server_block(Data &wbsv_data, Utils utils, std::string filename, int line_index)
{
    if (utils.key == "{"){
        wbsv_data.error = "WebServer: [emerg] unexpected \"{\" in ";
        wbsv_data.error += filename + ':' + ft_to_string(line_index);
    }
    else if (utils.key != "server")
    {
        wbsv_data.error = "WebServer: [emerg] unknown directive \"";
        wbsv_data.error += utils.key + "\" in " + filename + ':' + ft_to_string(line_index);
    }
    else {
        wbsv_data.error = "WebServer: [emerg] directive \"server\" has no opening \"{\" in ";
        wbsv_data.error += filename + ':' + ft_to_string(line_index);
    }
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
                wbsv_data.error = "WebServer: [emerg] invalida number of arguments in \"";
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
            wbsv_data.error = "WebServer: [emerg] invalid value \"" + variable + "\" in ";
            wbsv_data.error += this->filename + ':' + ft_to_string(this->line_index);
        }
        utils.value.erase(0, utils.index + 1);

    }
    trim(utils.value, " \t;");
    if (!utils.value.length() || !find_error)
    {
        wbsv_data.error = "WebServer: [emerg] invalid number of arguments in \"";
        wbsv_data.error += utils.key + "\" directive in " + this->filename + ':' + ft_to_string(this->line_index);
    }
    else if (utils.value.find(".html") == std::string::npos)\
    {
        wbsv_data.error = "WebServer: [emerg] " + utils.value + " is not a error file ('file.html') in \"";
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
    wbsv_data.error = "WebServer: [emerg] value \"" + error + "\" must be between 300 and 599 in ";
    wbsv_data.error += this->filename + ':' + ft_to_string(this->line_index);
    return (0);
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

void    Config::check_semicolons(Utils &utils, Data &wbsv_data)
{
    if (utils.key != "location" && utils.value.find_first_of(";") != (utils.value.length() - 1))
    {
        wbsv_data.error = "WebServer: [emerg] directive \"";
        wbsv_data.error += utils.key + "\" is not terminated by \";\" in " + this->filename + ':' + ft_to_string(this->line_index);
    }
    else
        trim(utils.value, ";");
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
        wbsv_data.error = "WebServer: [emerg] unexpected end of file, expecting\"}\" in ";
        wbsv_data.error += this->filename + ':' + ft_to_string(line_index);
    }
    return brace_stack.empty();
}

int main(int ac, char **av)
{
    Data    wbsv_data;


    if (ac != 2)
    {
        std::cout << "Error: 2 arguments needed\n";
        return 0;
    }
    wbsv_data.config.parsing_file(av[1], wbsv_data);
    if (!wbsv_data.error.empty())
    {
        std::cout << RED << wbsv_data.error << END_CLR << std::endl;
        return (1);
    }
}