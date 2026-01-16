#pragma once
#include "Command.hpp"

class PingCommand : public Command {
    public:
        PingCommand() : Command("PING", 1, 1, false) {};
        ~PingCommand() {};
        void execute(Client *client, std::vector<std::string> params, std::string trailing) {
            (void) trailing;
            client->sendMessage("PONG :" + params[0]);
        }
};