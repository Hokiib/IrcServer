#pragma once
#include "Command.hpp"

class PartCommand : public Command {
    public:
        PartCommand() : Command("PART", 1, 1, false) {};
        ~PartCommand() {};
        void execute(Client *client, std::vector<std::string> params, std::string trailing) {
			std::string name = params[0];
			Channel *channel = Server::get()->getChannel(name);
			if (channel == NULL){
                client->sendNumeric(ERR_NOSUCHCHANNEL, ":No such channel");
				return;
            }
            if (!channel->isUser(client)){
                client->sendNumeric(ERR_NOTONCHANNEL, ":You're not on that channel");
                return;
            }
			channel->leave(client, trailing);
        }
};