/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbolivar <sbolivar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:30 by pollo             #+#    #+#             */
/*   Updated: 2026/04/21 23:41:30 by sbolivar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>

class Channel; // Forward declaration to avoid circular dependency

class Client {

    private:
        
        std::string				_nickname;
		std::string				_username;
		std::string				_realname;
        int						_socket;
        std::vector<Channel*>	_channels;
		bool					registered;

    public:

		Client(const std::string nickname, const std::string& username, const int socket);
		~Client();

		int	getClientSocket();
		const std::string	getNickname() const;
		const std::string	getUsername() const;
		const std::string	getRealname() const;
		bool				registeredStatus() const;
		void				isRegistered();
		void				setRealname(std::string); 
		void				setNickname(std::string);
		void				setUsername(std::string);
		
};
#endif