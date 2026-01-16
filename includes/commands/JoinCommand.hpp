#pragma once
#include "Command.hpp"

class JoinCommand : public Command {
    public:
        JoinCommand() : Command("JOIN", 1, 2, false) {};
        ~JoinCommand() {};
        void execute(Client *client, std::vector<std::string> params, std::string trailing) {
            (void) trailing;
			std::string name = params[0];
            if (name == ":")
                return;
			Server::get()->computeChannel(name, client, params.size() == 1 ? "" : params[1]);
        }
};