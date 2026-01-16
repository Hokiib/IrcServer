#pragma once
#include "Command.hpp"

class WhoIsCommand: public Command {
	public:
		WhoIsCommand() : Command("WHOIS", 1, 1, false) {};
		~WhoIsCommand() {};
		void execute(Client *client, std::vector<std::string> params, std::string trailing) {
			(void) trailing;
			std::string nick = params[0];
			Client* target = Server::get()->getClient(nick);
			if (target == NULL) {
				client->sendNumeric(ERR_NOSUCHNICK, nick + " :No such user");
				client->sendNumeric(RPL_ENDOFWHOIS, nick + " :End of WHOIS");
				return;
			}
			client->sendNumeric(RPL_WHOISUSER, nick + " " + target->getUsername() + " " + target->getIp() + " * :" + target->getRealName());
			client->sendNumeric(RPL_ENDOFWHOIS, nick + " :End of WHOIS");
		}
};