#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ConfigUtils.hpp"
#include "Webserv.hpp"

class Data;
class Utils;
class ServerConf;

typedef std::vector<ServerConf> Serv_list;
typedef std::map<std::string, std::vector<std::string> > map_vector;
typedef std::vector<std::map<std::string, std::map<std::string, std::vector<std::string> > > >::iterator vector_location_it;
typedef std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator map_path_location_it;
typedef std::map<std::string, std::vector<std::string> >::iterator map_vector_it;

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
        void    printData(ServerConf server);
};

void ft_error_server_bloc(Data &wbsv_data, Utils utils, std::string filename, int line_index);

#endif