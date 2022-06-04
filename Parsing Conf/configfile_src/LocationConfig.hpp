/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrija <mbrija@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/17 00:32:50 by mbrija            #+#    #+#             */
/*   Updated: 2022/06/04 12:58:28 by mbrija           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "../global_src/support_funcs.hpp"

class LocationConfig
{
private:
    std::string name;
    std::string location_index;
    std::string root;
    std::vector<std::string> redirect;
    std::string upload;
    std::vector<std::string> methods;
    std::vector<std::string> cgi;
    bool auto_index;
    
public:
    LocationConfig(/* args */);
    LocationConfig(LocationConfig const & p);	
    ~LocationConfig();
    LocationConfig & operator= (LocationConfig const &p);
    //getters
    std::string get_name();
    std::string get_location_index();
    std::string get_root();
    std::vector<std::string> get_redirect();
    std::string get_upload();
    std::vector<std::string> getMethods();
    std::vector<std::string> getCgi();
    
       
friend std::ostream &operator<<(std::ostream &output, LocationConfig &D ) 
{ 
        output << "name : " << D.get_name() << std::endl;
        output << "location_index : " << D.get_location_index() << std::endl;
        output << "root : " << D.get_root() << std::endl;
        output << "upload : " << D.get_upload() << std::endl;
        // for (auto i: D.get_redirect())
        // {
        //     output << "redirect : " << i << std::endl;
        // }
        std::vector<std::string> redir = D.get_redirect();
        output << "redirect code : " << redir[0] << std::endl;
        output << "redirect link : " << redir[1] << std::endl;
        for (auto i: D.getMethods())
            std::cout << "Mathods " << i << ' ' << std::endl;
        for (auto i: D.getCgi())
            std::cout << "Cgi " << i << ' ' << std::endl;
        return output;
}

    //parse here
    void locationParser(std::string buf);

    class Error_exc : public std::exception {
    private:
        std::string error_msg;
    public:
        explicit Error_exc( const std::string &error_msg ) throw() : error_msg(error_msg) {}
        ~Error_exc() throw() {}

        virtual const char * what() const throw() {
            return (error_msg.c_str());
        }
    };
};

