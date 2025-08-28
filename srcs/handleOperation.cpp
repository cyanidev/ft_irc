
#include "Server.hpp"
#include "../includes/client.hpp"
#include "../includes/channel.hpp"


void	Server::handleJoin(const std::string& name, const std::string& topic, class client* creator) {
	if (name.empty() || topic.empty() || creator == NULL) {
		std::cerr << "Invalid channel parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#" + name) {
			std::cout << "Channel " << channels[i]->getChannelName() << " already exists" << std::endl;
			channels[i]->addUser(creator);
			return;
		}
	}
	channel *newChannel = new channel(name, topic, creator);
	channels.push_back(newChannel);
	channels[channels.size() - 1]->addUser(creator);
	channels[channels.size() - 1]->addOperator(creator);
}

void	Server::handleKick(const std::string& channel, class client* user) {
	if (channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			channels[i]->removeUser(user);
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	Server::handleInvite(const std::string& channel, class client* user) {
	if (channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			channels[i]->addUser(user);
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	Server::handleTopic(const std::string& channel, const std::string& topic) {
	if (channel.empty() || topic.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#"+channel) {
			channels[i]->setChannelTopic(topic);
			std::cout << "Topic for channel " << channel << " set to: " << topic << std::endl;
			return;
		}
	}
	std::cerr << "Channel " << channel << " not found" << std::endl;
}

void	Server::handleMode(const std::string& channel, const std::string& mode, bool enable) {
	if (channel.empty() || mode.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			if (mode == "i") {
				if (channels[i]->getInviteOnlyChannel() != enable) {
					channels[i]->setInviteOnlyChannel(enable);
				}
				std::cout << "Invite-only mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "t") {
				if (channels[i]->getRestrictedTopic() != enable) {
					channels[i]->setRestrictedTopic(enable);
				}
				std::cout << "Restricted topic mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "k") {
				if (channels[i]->getChannelKeyRequired() != enable) {
					channels[i]->setChannelKeyRequired(enable);
				}
				std::cout << "Channel key required mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "o") {
				if (channels[i]->getChannelOperatorPrivilege() != enable) {
					channels[i]->setChannelOperatorPrivilege(enable);
				}
				std::cout << "Channel operator privilege mode for channel " << channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else
				std::cerr << "Unknown mode: " << mode << std::endl;
			return;
		}
	}
	std::cout << "Channel " << channel << " not found" << std::endl;
}

void	Server::handleMode(const std::string& channel, const std::string& mode, int number) {
	if (channel.empty() || mode.empty() || mode != "l" || number < 0) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == channel) {
			if (number != channels[i]->getMaxUsers()) {
				channels[i]->setMaxUsers(0);
				std::cout << "Max users for channel " << channel << " set to: " << number << std::endl;
			}
			else {
				std::cout << "Max users for channel " << channel << " already set to: " << number << std::endl;
			}
			return;
		}
		std::cout << "Channel " << channel << " not found" << std::endl;
	}
}

