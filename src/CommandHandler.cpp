#include "CommandHandler.hpp"

#include "commands/CapCommand.hpp"
#include "commands/NickCommand.hpp"
#include "commands/PingCommand.hpp"
#include "commands/UserCommand.hpp"
#include "commands/ModeCommand.hpp"
#include "commands/JoinCommand.hpp"
#include "commands/PartCommand.hpp"
#include "commands/PrivateMessageCommand.hpp"
#include "commands/WhoIsCommand.hpp"
#include "commands/QuitCommand.hpp"
#include "commands/PassCommand.hpp"
#include "commands/KickCommand.hpp"
#include "commands/TopicCommand.hpp"
#include "commands/InviteCommand.hpp"

CommandHandler::CommandHandler(void) {
	this->registerCommands();
}

CommandHandler::~CommandHandler(void) {
	std::map<std::string, Command*>::iterator it = this->commands.begin();

	while (it != this->commands.end()) {
		delete (*it).second;
		it++;
	}
}

void CommandHandler::registerCommands(void) {
	std::vector<Command*> commands;
	commands.push_back(new CapCommand());
	commands.push_back(new NickCommand());
	commands.push_back(new PingCommand());
	commands.push_back(new UserCommand());
	commands.push_back(new ModeCommand());
	commands.push_back(new JoinCommand());
	commands.push_back(new PartCommand());
	commands.push_back(new PrivateMessageCommand());
	commands.push_back(new WhoIsCommand());
	commands.push_back(new QuitCommand());
	commands.push_back(new PassCommand());
	commands.push_back(new KickCommand());
	commands.push_back(new TopicCommand());
	commands.push_back(new InviteCommand());

	for (size_t i = 0; i < commands.size(); i++) {
		Command* command = commands.at(i);
		this->commands[command->getName()] = command;
	}
}

void CommandHandler::execute(Client *client, std::string command, std::vector<std::string> params, std::string trailing) {
	if (command.length() == 0)
		return;
	Logger::debug("");
	Logger::debug("Client " + Utils::toString(client->getFd()));
	Logger::debug("Executing \"" + command + "\"");
	if (params.size() > 0) {
		Logger::debug("Params : [", false);
		std::vector<std::string>::iterator it = params.begin();
		while (it != params.end()){
			if (it != params.begin())
				Logger::debug(" ", false);
			Logger::debug(*it, false);
			it++;
		}
		Logger::debug("]", true);
	}
	if (trailing.size() != 0)
		Logger::debug("Trailing : {" + trailing + "}");
	
	try {
		Command *cmd = this->commands.at(command);
		if (cmd->mustBeConnected() && !client->isConnected())
			return;
		if (!cmd->canExecuted(params, trailing)) {
			Server::get()->alert(client, "Bad syntax : " + command);
			return;
		}
		cmd->execute(client, params, trailing);
	} catch (std::out_of_range &e) {
		Logger::debug(YELLOW "Unknown command : " + command);
	}
}