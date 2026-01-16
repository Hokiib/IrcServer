#pragma once
#include "Command.hpp"

class NickCommand : public Command
{
	public:
		NickCommand() : Command("NICK", 1, 1, false, false) {};
		~NickCommand() {};
		void execute(Client *client, std::vector<std::string> params, std::string trailing) {
			(void)trailing;
			std::string nick = params[0];
			if (Server::get()->isNickExist(nick, client))
			{
				client->sendNumeric(ERR_NICKNAMEINUSE, nick + " :Nickname is already in use");
				return ;
			}
			if (client->isConnected())
				Server::get()->broadcast(client->formatName() + " NICK :" + nick);
			client->setNick(nick);
			if (!client->isConnected() && client->getUsername().size() != 0)
				client->connect();
		}
};
