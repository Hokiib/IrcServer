#pragma once
#include "Command.hpp"

class PrivateMessageCommand: public Command {
	public:
		PrivateMessageCommand(): Command("PRIVMSG", 1, 1, true){};
		~PrivateMessageCommand(){};
		void execute(Client *client, std::vector<std::string> params, std::string trailing){
			std::string name = params[0];
			std::string message = client->formatName() + " PRIVMSG " + name + " :" + trailing;

			// User
			if (name[0] != '#') {
				Client* target = Server::get()->getClient(name);
				if (target == NULL) {
					client->sendNumeric(ERR_NOSUCHNICK, name + " :No such user");
					return;
				}
				target->sendMessage(message);
				return ;
			}

			// Channel
			Channel *channel = Server::get()->getChannel(name);
			if (channel == NULL){
				client->sendNumeric(ERR_NOSUCHCHANNEL, ":No such channel");
				return;
			}
			if (!channel->isUser(client)){
				client->sendNumeric(ERR_NOTONCHANNEL, ":You're not on that channel");
				return;
			}
			channel->sendMessage(client, message);
		}
};