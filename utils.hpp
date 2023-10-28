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

# define WHITE_SPACE " \t"
# define NUM "0123456789"
# define IP_NUM "0123456789:."
# define RED "\x1b[31m"
# define END_CLR "\033[0m"
# define SIZE_TYPE "MKGBmkgb"
# define ERROR_PATH "./error.html"

class ServerConf;

typedef std::vector<ServerConf> Serv_list;
typedef std::vector<std::map<std::string, std::map<std::string, std::vector<std::string> > > >::iterator vector_location_it;
typedef std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator map_path_location_it;
typedef std::map<std::string, std::vector<std::string> >::iterator map_vector_it;
typedef std::map<std::string, std::vector<std::string> > map_vector;

void    trim(std::string& string, std::string value)
{
    size_t first = string.find_first_not_of(value);
	string.erase(0, first);
	size_t last = string.find_last_not_of(value);
	size_t len = string.length() - (last - 1);
	string.erase(last + 1, len);
}

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

std::string ft_to_string(int entier)
{
    std::stringstream buf;
    buf << entier;
    return buf.str();
}


int valid_vector_size( std::vector<std::string> vec, size_t check)
{
    if (vec.size() > check || vec.size() == 0)
        return (0);
    return (1);
}

bool is_char_in_str(std::string str, char c)
{
    return (str.find(c) != std::string::npos);
}

void convert_body_size(std::string &size, std::string number, char type)
{
    
    std::ostringstream s_num;
    long num = std::atoi(number.c_str());

    if (type == 'G' || type == 'g')
        s_num << (num * 1024 * 1024 * 1024);
    else if (type == 'M' || type == 'm')
        s_num << (num * 1024 * 1024);
    else if (type == 'K' || type == 'k')
        s_num << (num * 1024);
    else if (type == 'B' || type == 'b')
        s_num << num;
    size = s_num.str();
}

int valid_vector_return_size(std::vector<std::string> vec) 
{
    if ((vec.size() % 2) != 0 || (vec.size() == 0))
        return (0);
    return (1);
}

bool check_exist_server_data(map_vector server_data, std::string key)
{
    if (server_data.count(key) > 0)
        return (true);
    return (false);
}