/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollo <pollo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:30 by pollo             #+#    #+#             */
/*   Updated: 2025/08/21 15:27:28 by pollo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>

class Server;
class Channel;

class Client {

    private:
        
        std::string				_nickname;
		std::string				_username;
        int						_socket;
        std::vector<channel*>	_channels;

    public:

		Client(const std::string nickname, const std::string& username, const int socket);
		~Client();

		int	getClientSocket();
		const std::string	getNickname() const;
		const std::string	getUsername() const;
		
};
