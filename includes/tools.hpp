#ifndef TOOLS_HPP
#define TOOLS_HPP

#include "Webserv.hpp"

void    		trim(std::string& string, std::string value);
std::string 	ft_to_string(int entier);
int 			valid_vector_size( std::vector<std::string> vec, size_t check);
bool 			is_char_in_str(std::string str, char c);
void 			convert_body_size(std::string &size, std::string number, char type);
int 			valid_vector_return_size(std::vector<std::string> vec);

#endif