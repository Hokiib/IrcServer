#pragma once
#include "Command.hpp"

class TopicCommand : public Command {
    public:
        TopicCommand() : Command("TOPIC", 1, 1, true) {};
        ~TopicCommand() {};
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
			if (channel->isTopicOperator() && !channel->isOp(client)) {
				client->sendNumeric(ERR_CHANOPRIVSNEEDED, name + " :You're not channel operator");
				return;
			}
			channel->setTopic(trailing, client);
        }
};