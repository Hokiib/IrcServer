#pragma once
#include "Command.hpp"

class PassCommand: public Command {
	public:
		PassCommand(): Command("PASS", 1, 1, false, false){};
		~PassCommand(){};
		void execute(Client *client, std::vector<std::string> params, std::string trailing){
			(void) trailing;
			if (client->isConnected() || client->isRegistered())
				return ;
			std::string input = params[0];
			if (input != Server::get()->getPassword()) {
				client->sendError("Wrong password");
				Server::get()->disconnect(client->getFd(), "");
			} else
				client->setRegistered();
		}
};