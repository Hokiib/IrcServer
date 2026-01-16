#pragma once
#include "Command.hpp"

class KickCommand : public Command {
    public:
        KickCommand() : Command("KICK", 1, 2, false) {};
        ~KickCommand() {};
        void execute(Client *client, std::vector<std::string> params, std::string trailing) {
			std::string name = params[0];
			Channel *channel = Server::get()->getChannel(name);
			if (channel == NULL){
                client->sendNumeric(ERR_NOSUCHCHANNEL, name + " :No such channel");
				return;
            }
            if (!channel->isUser(client)){
                client->sendNumeric(ERR_NOTONCHANNEL, name + " :You're not on that channel");
                return;
            }
			if (!channel->isOp(client)) {
				client->sendNumeric(ERR_CHANOPRIVSNEEDED, name + " :You're not channel operator");
				return;
			}
			std::string user = params[1];
			Client* target = Server::get()->getClient(user);
			if (target == NULL) {
				client->sendNumeric(ERR_NOSUCHNICK, user + " :No such user");
				return;
			}
			if(!channel->isUser(target)) {
				client->sendNumeric(ERR_USERNOTINCHANNEL, user + " " + name + " :He's not on that channel");
				return;
			}
			channel->kick(target, client, trailing);
        }
};