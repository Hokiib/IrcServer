#pragma once
#include "Command.hpp"

class ModeCommand : public Command {
	public:
		ModeCommand() : Command("MODE", 1, 15, false) {};
		~ModeCommand() {};
		void execute(Client *client, std::vector<std::string> params, std::string trailing) {
			(void) trailing;
			std::string target = params[0];
			if (target.size() == 0)
				return ;
			if (target[0] != '#')
				return;
			Channel* channel = Server::get()->getChannel(target);
			if (channel == NULL) {
				client->sendNumeric(ERR_NOSUCHCHANNEL, ":No such channel");
				return;
			}
			
			if (params.size() == 1) {
				std::stringstream letters;
				std::stringstream values;
				
				if (channel->isInviteOnly())
					letters << "i";
				if (channel->isTopicOperator())
					letters << "t";
				if (channel->getPassword().length() > 0) {
					letters << "k";
					values << " " + channel->getPassword();
				}
				if (channel->getUserLimit() != 0) {
					letters << "l";
					values << " " << Utils::toString(channel->getUserLimit());
				}
				if (letters.str().size() == 0)
					return;

				client->sendNumeric(RPL_CHANNELMODEIS, channel->getName() + " +" + letters.str() + values.str());
			} else {
				if (!channel->isOp(client)) {
					client->sendNumeric(ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
					return;
				}
				std::stringstream first, second;
				
				bool plus = true;
				size_t j = 2;
				std::string letters = params[1];
				for (size_t i = 0; i < letters.size(); i++) {
					switch (letters[i]) {
						case '+':
							if (plus)
								break;
							first << '+';
							plus = true;
							break;
						case '-':
							if (!plus)
								break;
							first << '-';
							plus = false;
							break;
						case 'i':
							if (plus == channel->isInviteOnly())
								break;
							first << 'i';
							channel->setInviteOnly(plus);
							break;
						case 't':
							if (plus == channel->isTopicOperator())
								break;
							first << 't';
							channel->setTopicOperator(plus);
							break;
						case 'k': case 'o':
							if (params.size() <= j) {
								client->sendNumeric(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
								break;
							}
							switch(letters[i]) {
								case 'k':
									if (!plus && channel->getPassword().size() == 0)
										break;
									if (plus && channel->getPassword() == params[j])
										break;
									first << 'k';
									if (!plus)
										channel->setPassword("");
									else {
										channel->setPassword(params[j]);
										second << " " + params[j];
										j++;
									}
									break;
								case 'o':
									Client* target = Server::get()->getClient(params[j]);
									if (target == NULL) {
										client->sendNumeric(ERR_NOSUCHNICK, params[j] + " :No such user");
										break;
									}
									if (!channel->isUser(target)) {
										client->sendNumeric(ERR_USERNOTINCHANNEL, params[j] + " " + channel->getName() + " :He's not on that channel");
										break;
									}
									if (client == target)
										break;
									if (plus == channel->isOp(target))
										break;
									channel->setOp(target, client->getNick(), plus);
									j++;
									break;
							}
							break;
						case 'l':
							if (plus && params.size() <= j) {
								client->sendNumeric(ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
								break;
							}
							if (plus) {
								std::istringstream iss(params[j]);
								size_t limit;
								iss >> std::noskipws >> limit;
								if (!iss.eof() || iss.fail() || !std::isdigit(params[j][0]))
									break;
								if (limit == channel->getUserLimit())
									break;
								first << 'l';
								second << " " + Utils::toString(limit);
								channel->setUserLimit(limit);
								j++;
							} else {
								if (channel->getUserLimit() == 0)
									break;
								first << 'l';
								channel->setUserLimit(0);
							}
							break;
						default:
							client->sendNumeric(ERR_UNKNOWNMODE, Utils::toString(letters[i]) + " :is unknown mode char");
							break;
					}
				}
				std::string modes = first.str();
				if (modes.size() == 0)
					return;
				bool valid = false;
				for (size_t i = 0; i < modes.size(); i++) {
					if (modes[i] != '-' && modes[i] != '+') {
						valid = true;
						break;
					}
				}
				if (!valid)
					return;
				channel->broadcastMode(client->getNick(), modes + second.str());
			}
		}
};