#pragma once
#include "Server.hpp"
#include "Command.hpp"

class CommandHandler {
	private:
		std::map<std::string, Command*> commands;
	public:
		CommandHandler();
		~CommandHandler();
		void registerCommands(void);
		void execute(Client *client, std::string command, std::vector<std::string> params, std::string trailing);
};