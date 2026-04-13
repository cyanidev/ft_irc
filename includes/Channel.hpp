/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Andie <Andie@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 13:13:13 by pollo             #+#    #+#             */
/*   Updated: 2026/04/13 17:27:43 by Andie            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>
#include <set>
#include "Client.hpp"
#include "Server.hpp"

class Channel {

    private:
        
        std::string				_name;
		std::string				_topic;
        std::vector<Client*>	_users;
		std::set<Client*>		_operators;
		std::set<Client*>		_bannedUsers;

		bool					_inviteOnlyChannel;
		bool					_restrictedTopic;
		bool					_channelKeyRequired;
		bool					_channelOperatorPrivilege;
		int						_maxUsers;

    public:	

		Channel(const std::string& name, const std::string& topic, Client* creator);
		~Channel();

		const std::string	getChannelName();
		const std::string	getChannelTopic();

		void	setChannelTopic(const std::string& topic);

		void	addUser(Client* user);
		void	addOperator(Client* user);
		void	removeOperator(Client* user);
		void	removeUser(Client* user);
		void	banUser(Client* user);

		bool	getInviteOnlyChannel() const;
		bool	getRestrictedTopic() const;
		bool	getChannelKeyRequired() const;
		bool	getChannelOperatorPrivilege() const;
		int		getMaxUsers() const;
		int		getCurrentUsers() const;

		void	setInviteOnlyChannel(bool inviteOnly);
		void	setRestrictedTopic(bool restricted);
		void	setChannelKeyRequired(bool keyRequired);
		void	setChannelOperatorPrivilege(bool operatorPrivilege);
		void	setMaxUsers(int maxUsers);
		
};

#endif