#pragma once
#include "Command.hpp"

class CapCommand: public Command {
	public:
		CapCommand(): Command("CAP", 1, 2, false, false){};
		~CapCommand(){};
		void execute(Client *client, std::vector<std::string> params, std::string trailing){
			(void) trailing;
			if (client->isConnected())
				return ;

			if (params[0] == "LS") {
				client->sendServerMessage("CAP * LS");
				return ;
			} else if (params[0] == "END")
				return ;
		}
};