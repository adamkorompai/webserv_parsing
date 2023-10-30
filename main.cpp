/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorompa <akorompa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:35:12 by akorompa          #+#    #+#             */
/*   Updated: 2023/10/30 15:29:51 by akorompa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Webserv.hpp"


int main(int ac, char **av)
{
    Data    wbsv_data;


    if (ac != 2)
        wbsv_data.config.parsing_file("config", wbsv_data);
    else if (ac == 2)
        wbsv_data.config.parsing_file(av[1], wbsv_data);
    if (!wbsv_data.error.empty())
    {
        std::cout << RED << wbsv_data.error << END_CLR << std::endl;
        return (1);
    }
}