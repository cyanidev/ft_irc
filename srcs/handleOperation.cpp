
#include "Server.hpp"
#include "../includes/Client.hpp"
#include "Channel.hpp"


void	Server::handleJoin(const std::string& name, const std::string& topic, class Client* creator) {
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
	Channel *newChannel = new Channel(name, topic, creator);
	channels.push_back(newChannel);
	channels[channels.size() - 1]->addUser(creator);
	channels[channels.size() - 1]->addOperator(creator);
}

void	Server::handleKick(const std::string& Channel, class Client* user) {
	if (Channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#" + Channel) {
			channels[i]->removeUser(user);
			return;
		}
	}
	std::cerr << "Channel " << Channel << " not found" << std::endl;
}

void	Server::handleInvite(const std::string& Channel, class Client* user) {
	if (Channel.empty() || user == NULL) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#" + Channel) {
			channels[i]->addUser(user);
			return;
		}
	}
	std::cerr << "Channel " << Channel << " not found" << std::endl;
}

void	Server::handleTopic(const std::string& Channel, const std::string& topic) {
	if (Channel.empty() || topic.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#" + Channel) {
			channels[i]->setChannelTopic(topic);
			std::cout << "Topic for channel " << Channel << " set to: " << topic << std::endl;
			return;
		}
	}
	std::cerr << "Channel " << Channel << " not found" << std::endl;
}

void	Server::handleMode(const std::string& Channel, const std::string& mode, bool enable) {
	if (Channel.empty() || mode.empty()) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == Channel) {
			if (mode == "i") {
				if (channels[i]->getInviteOnlyChannel() != enable) {
					channels[i]->setInviteOnlyChannel(enable);
				}
				std::cout << "Invite-only mode for channel " << Channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "t") {
				if (channels[i]->getRestrictedTopic() != enable) {
					channels[i]->setRestrictedTopic(enable);
				}
				std::cout << "Restricted topic mode for channel " << Channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "k") {
				if (channels[i]->getChannelKeyRequired() != enable) {
					channels[i]->setChannelKeyRequired(enable);
				}
				std::cout << "Channel key required mode for channel " << Channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else if (mode == "o") {
				if (channels[i]->getChannelOperatorPrivilege() != enable) {
					channels[i]->setChannelOperatorPrivilege(enable);
				}
				std::cout << "Channel operator privilege mode for channel " << Channel << " set to: " << (enable ? "true" : "false") << std::endl;
			}
			else
				std::cerr << "Unknown mode: " << mode << std::endl;
			return;
		}
	}
	std::cout << "Channel " << Channel << " not found" << std::endl;
}

void	Server::handleMode(const std::string& Channel, const std::string& mode, int number) {
	if (Channel.empty() || mode.empty() || mode != "l" || number < 0) {
		std::cerr << "Invalid parameters" << std::endl;
		return;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]->getChannelName() == "#" + Channel) {
			if (number != channels[i]->getMaxUsers()) {
				channels[i]->setMaxUsers(number);
				std::cout << "Max users for Channel " << Channel << " set to: " << number << std::endl;
			}
			else {
				std::cout << "Max users for Channel " << Channel << " already set to: " << number << std::endl;
			}
			return;
		}
	}
	std::cout << "Channel " << Channel << " not found" << std::endl;
}

