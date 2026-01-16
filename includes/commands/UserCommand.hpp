#pragma once
#include "Command.hpp"

class UserCommand: public Command {
	public:
		UserCommand() : Command("USER", 3, 3, true, false) {};
		~UserCommand() {};
		void execute(Client *client, std::vector<std::string> params, std::string trailing) {
			client->setName(params[0], trailing);
			if (!client->isConnected() && client->getNick() != "*")
				client->connect();
		}
};