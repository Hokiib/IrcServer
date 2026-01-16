#include "Channel.hpp"

Channel::Channel(std::string name, Client *owner){
	this->name = name;
	this->operators.push_back(owner);
	this->limit = 0;
	this->topic = "Channel of " + owner->getNick();
	this->inviteOnly = false;
	this->topicOperator = false;
	this->join(owner, "");
}

Channel::~Channel() {
	for (size_t i = 0; i < this->users.size(); i++)
		this->leave(this->users.at(i), "Channel closed");
	Logger::info(RED "Channel " + this->name + " was deleted");
}

void Channel::join(Client *client, std::string password){
	if (!this->isOp(client) && this->isInviteOnly() && !this->isInvited(client)) {
		client->sendNumeric(ERR_INVITEONLYCHAN, this->name + " :This channel is on invite-only");
		return;
	}
	if (this->isFull()) {
		client->sendNumeric(ERR_CHANNELISFULL, this->name + " :This channel is full");
		return;
	}
	if (!this->isInvited(client) && this->password.size() > 0 && this->password != password) {
		client->sendNumeric(ERR_BADCHANNELKEY, this->name + " :Wrong password");
		return;
	}
	client->sendNumeric(RPL_TOPIC, this->name + " :" + this->topic);
	this->clearInvitation(client);
	if (this->getUser(client) != this->users.end())
		return;
	if (this->isOp(client))
		Logger::info(YELLOW "+" BLUE " Client " + Utils::toString(client->getFd()) + " created " + this->name);
	else
		Logger::info(GREEN "→" BLUE " Client " + Utils::toString(client->getFd()) + " joined " + this->name);
	this->users.push_back(client);
	this->broadcast(client->formatName() + " JOIN :" + this->name);
	iterator it = this->users.begin();

	std::stringstream names;
	while (it != this->users.end()) {
		Client *client = *it;
		if (it != this->users.begin())
			names << " ";
		if (this->isOp(client))
			names << "@";
		names << client->getNick();
		it++;
	}

	std::string status = this->inviteOnly ? "*" : "=";
	client->sendNumeric(RPL_NAMREPLY, status + " " + this->name + " :" + names.str());
	client->sendNumeric(RPL_ENDOFNAMES, this->name + " :End of NAMES list");
}

void Channel::removeClient(Client *client) {
	if (Server::get()->isClosing())
		return;
	iterator it = this->getUser(client);
	if (it == this->users.end())
		return ;
	this->users.erase(it);
	it = this->getOperator(client);
	
	if (it == this->operators.end())
		return ;
	this->operators.erase(it);
	if (this->operators.size() > 0)
		return ;
	if (this->users.size() == 0) {
		Server::get()->deleteChannel(this);
		return;
	}
	this->setOp(this->users[0], "", true);
}

void Channel::leave(Client *client, std::string message) {
	if (!this->isUser(client)) {
		this->clearInvitation(client);
		return;
	}
	Logger::info(RED "←" BLUE " Client " + Utils::toString(client->getFd()) + " left " + this->name);
	this->broadcast(client->formatName() + " PART " + this->name + (message.size() == 0 ? "" : " :" + message));
	this->removeClient(client);
}

void Channel::kick(Client *client, Client *by, std::string reason){
	if (!this->isUser(client))
		return;
	Logger::info(RED "←" BLUE " Client " + Utils::toString(client->getFd()) + " has been kicked from " + this->name + "." + (reason.size() == 0 ? "" : " Reason : " + reason));
	this->broadcast(by->formatName() + " KICK " + this->name + " " + client->getNick() + (reason.size() == 0 ? "" : " :" + reason));
	this->removeClient(client);
}

void Channel::broadcast(std::string message) {
	for (size_t i = 0; i < this->users.size(); i++)
		this->users[i]->sendMessage(message);
}

void Channel::broadcastMode(std::string from, std::string message) {
	if (from.length() == 0)
		from = "[SERVER]";
	this->broadcast(":" + from + " MODE " + this->name + " " + message);
}

void Channel::sendMessage(Client* from, std::string message) {
	for (size_t i = 0; i < this->users.size(); i++) {
		Client* client = this->users[i];
		if (client == from)
			continue;
		client->sendMessage(message);
	}
}

std::string Channel::getName() {
	return this->name;
}

void Channel::setTopic(std::string topic, Client *by){
	this->topic = topic;
	this->broadcast(by->formatName() + " TOPIC " + this->name + " :" + topic);
}

std::string Channel::getTopic(){
	return this->topic;
}

void Channel::setUserLimit(size_t limit) {
	if (limit <= 0)
		this->limit = 0;
	else
		this->limit =  limit;
}

size_t Channel::getUserLimit() {
	return this->limit;
}

void Channel::setInviteOnly(bool inviteOnly) {
	this->inviteOnly = inviteOnly;
	this->invited.clear();
}

bool Channel::isInviteOnly() {
	return this->inviteOnly;
}

void Channel::clearInvitation(Client* client) {
	iterator invited = this->getInvited(client);
	if (invited == this->invited.end())
		return;
	this->invited.erase(invited);
}

void Channel::invite(Client* client, Client* by) {
	if (!this->isInvited(client) && this->inviteOnly)
		this->invited.push_back(client);
	client->sendMessage(by->formatName() + " INVITE " + client->getNick() + " " + this->name);
	by->sendNumeric(RPL_INVITING, client->getNick() + " " + this->name);
}

bool Channel::isFull() {
	if (this->limit == 0)
		return false;
	return this->users.size() >= this->limit;
}

void Channel::setOp(Client *client, std::string by, bool op) {
	if (this->isOp(client) == op)
		return ;
	if (op) {
		this->operators.push_back(client);
		this->broadcastMode(by, "+o " + client->getNick());
		Logger::info(YELLOW "★" GREEN " Client " + Utils::toString(client->getFd()) + " is now operator in " + this->name);
	} else {
		this->operators.erase(this->getOperator(client));
		this->broadcastMode(by, "-o " + client->getNick());
		Logger::info(YELLOW "☆" GREEN " Client " + Utils::toString(client->getFd()) + " is not operator anymore in " + this->name);
	}

}

bool Channel::isOp(Client *client) {
	return this->getOperator(client) != this->operators.end();
}

bool Channel::isTopicOperator() {
	return this->topicOperator;
}

void Channel::setTopicOperator(bool topicOperator) {
	this->topicOperator = topicOperator;
}

void Channel::setPassword(std::string password) {
	this->password = password;
}

std::string Channel::getPassword() {
	return this->password;
}

bool Channel::isUser(Client *client) {
	return this->getUser(client) != this->users.end();
}

bool Channel::isInvited(Client *client) {
	return this->getInvited(client) != this->invited.end();
}

iterator Channel::getUser(Client *client) {
	iterator it = this->users.begin();
	while (it != this->users.end()) {
		if (*it == client)
			return it;
		it++;
	}
	return it;
}

iterator Channel::getOperator(Client *client) {
	iterator it = this->operators.begin();
	while (it != this->operators.end()) {
		if (*it == client)
			return it;
		it++;
	}
	return it;
}

iterator Channel::getInvited(Client *client) {
	iterator it = this->invited.begin();
	while (it != this->invited.end()) {
		if (*it == client)
			return it;
		it++;
	}
	return it;
}