/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Andie <Andie@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:30 by pollo             #+#    #+#             */
/*   Updated: 2026/05/07 19:27:18 by Andie            ###   ########.fr       */
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
#include <string>

class Channel; // Forward declaration to avoid circular dependency

class Client {

    private:
        
        std::string				_nickname;
		std::string				_username;
		std::string				_realname;
		std::string				_password;
        int						_socket;
        std::vector<Channel*>	_channels;
		bool					_registered;
		
		std::string				_recvBuffer;
		std::string				_sendBuffer;
		

    public:

		Client(const std::string nickname, const std::string& username, const int socket);
		~Client();
		
		// socket
		int	getClientSocket() const;
		
		// getters de indentidad
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		const std::string&	getRealname() const;
		const std::string&	getPassword() const;
		
		//setters de indentidad
		void				setPassword(const std::string& pass);
		void				setRealname(const std::string& real); 
		void				setNickname(const std::string& nick);
		void				setUsername(const std::string& user);

		// registro
		bool				isRegistered() const;
		void				setRegistered(bool val);

		//canales
		void				addChannel(Channel* channel);
		void				removeChannel(Channel* channel);
		const std::vector<Channel*>& getChannels() const;
		
		// buffer de receocion
		std::string	getRecvBuffer() const;
		void				appendToRecvBuffer(const std::string& data);
		void				clearRecvBuffer();

		// buffer de envio
		void	sendMessage(const std::string& message);
		void	flushSendBuffer();
		bool	hasPendingMessages() const;
		void	appendSendBuffer(const std::string& msg);
		std::string& getSendBuffer();
		
};
#endif