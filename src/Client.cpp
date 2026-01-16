#include "Client.hpp"

Client::Client(int fd, std::string ip) {
	this->fd = fd;
	this->ip = ip;
	this->nick = "*";
	this->connected = false;
	this->registered = false;
}

Client::~Client() {
	this->disconnect();
}

void Client::connect() {
	if (this->connected)
		return;
	if(!this->registered) {
		this->sendError("You have not entered password");
		Server::get()->disconnect(this->fd, "");
		return;
	}
	this->sendNumeric(RPL_WELCOME, "Welcome to the IRC network");
	Logger::success("Client " + Utils::toString(this->fd) + " (" + this->ip + ") connected");
	this->connected = true;
}

void Client::disconnect() {
	if (this->fd != -1) {
		
		close(this->fd);
		if (this->connected)
			Logger::warning("Client " + Utils::toString(this->fd) + " disconnected");
		this->fd = -1;
	}
}

void Client::push(char* buff, ssize_t bytes) {
	this->buffer.append(buff, bytes);

	size_t index;
	std::string match = EOL;

	int fd = this->fd;

	while ((index = this->buffer.find(match)) != std::string::npos) {
		std::string line = this->buffer.substr(0, index);
		Server::get()->processLine(this, line);
		if (Server::get()->getClient(fd) == NULL)
			break;
		this->buffer.erase(0, index + match.length());
	}
}

void Client::sendServerMessage(std::string message) {
	this->sendMessage(":" + Utils::toString(SERVER_NAME) + " " + message);
}

void Client::sendNumeric(IrcCode code, std::string message) {
	std::string result = "000";
	std::string casted = Utils::toString(code);
	for (size_t i = 0; i < casted.size(); i++)
		result[i + (result.size() - casted.size())] = casted[i];
	this->sendMessage(":" + Utils::toString(SERVER_NAME) + " " + result + " " + this->nick + " " + message);
}

void Client::sendMessage(std::string message) {
	if (this->fd == -1)
		return;
	Logger::debug("Sending \"" + message + "\" to Client " + Utils::toString(this->fd));
	send(this->fd, (message + EOL).c_str(), message.size() + 2, MSG_NOSIGNAL);
}

void Client::sendError(std::string error){
	this->sendMessage("ERROR :" + error);
}

int Client::getFd() {
	return this->fd;
}

std::string Client::getIp() {
	return this->ip;
}

std::string Client::getNick() {
	return this->nick;
}

void Client::setNick(std::string nick) {
	this->nick = nick;
}

std::string Client::getUsername() {
	return this->username;
}

std::string Client::getRealName() {
	return this->realname;
}

void Client::setName(std::string username, std::string realname) {
	this->username = username;
	this->realname = realname;
}

bool Client::isConnected() {
	return this->connected;
}

std::string Client::formatName() {
	return ":" + this->nick + "!" + this->username + "@" + this->ip;
}

void Client::setRegistered() {
	this->registered = true;
}

bool Client::isRegistered() {
	return this->registered;
}