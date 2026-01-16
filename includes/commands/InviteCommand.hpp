#pragma once
#include "Command.hpp"

class InviteCommand : public Command {
    public:
        InviteCommand() : Command("INVITE", 1, 2, false) {};
        ~InviteCommand() {};
        void execute(Client *client, std::vector<std::string> params, std::string trailing) {
			(void) trailing;

			std::string name = params[1];
			Channel *channel = Server::get()->getChannel(name);
			if (channel == NULL){
                client->sendNumeric(ERR_NOSUCHCHANNEL, name + " :No such channel");
				return;
            }
            if (!channel->isUser(client)){
                client->sendNumeric(ERR_NOTONCHANNEL, name + " :You're not on that channel");
                return;
            }
			if (channel->isInviteOnly() && !channel->isOp(client)) {
				client->sendNumeric(ERR_CHANOPRIVSNEEDED, name + " :You're not channel operator");
				return;
			}
			std::string user = params[0];
			Client* target = Server::get()->getClient(user);
			if (target == NULL) {
				client->sendNumeric(ERR_NOSUCHNICK, user + " :No such user");
				return;
			}
			if(channel->isUser(target)) {
				client->sendNumeric(ERR_USERONCHANNEL, user + " " + name + " :He's on that channel");
				return;
			}
			channel->invite(target, client);
        }
};