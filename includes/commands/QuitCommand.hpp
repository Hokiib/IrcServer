#pragma once
#include "Command.hpp"

class QuitCommand : public Command {
    public:
        QuitCommand() : Command("QUIT", 0, 0, false, false) {};
        ~QuitCommand() {};
        void execute(Client *client, std::vector<std::string> params, std::string trailing) {
            (void) params;
            Server::get()->disconnect(client->getFd(), trailing);
        }
};