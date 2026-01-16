#include "Server.hpp"

bool Server::shouldClose = false;
Server* Server::instance = NULL;

Server::Server(void) {
	Server::instance = this;
	this->fd = -1;
	this->epoll = -1;
	this->closed = false;
	this->commandHandler = NULL;
}

Server::~Server(void) {
	//close if not closed
	this->closeServer();
	Server::instance = NULL;
}

void Server::handleSignal(int s) {
	(void) s;
	Server::shouldClose = true;
}

void Server::start(uint16_t port, std::string password) {
	this->port = port;
	this->password = password;
	Logger::info(BLUE "Starting server on " + Utils::toString(port) + "...");
	signal(SIGINT, handleSignal); // CTRL + C //
	signal(SIGQUIT, handleSignal); // CTRL + \ //
	this->createSocket();
	Logger::success("Server started : " + Utils::toString(this->fd));
	this->live();
}

void Server::closeServer(void) {
	if (this->closed)
		return;

	if (this->commandHandler != NULL)
		delete this->commandHandler;

	// Close channels
	{
		std::map<std::string, Channel*>::iterator it = this->channels.begin();
		while (it != this->channels.end()) {
			delete it->second;
			it++;
		}
	}

	// Close clients
	{
		std::map<int, Client*>::iterator it = this->clients.begin();
		while (it != this->clients.end()) {
			Client* client = it->second;
			client->sendServerMessage("KILL " + client->getNick() + " :Server is shutting down");
			delete client;
			it++;
		}
	}

	if (this->fd != -1) {
		close(this->fd);
		if (this->epoll != -1)
			close(this->epoll);
		Logger::success("Server closed");
	}
	this->closed = true;
}

void Server::createEpoll(int fd) {
	// EPOLLIN = pret pour lire
	// EPOLLOUT = pret pour ecrire
	// autre = erreur en gros
	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;
	if (epoll_ctl(this->epoll, EPOLL_CTL_ADD, fd, &event) == -1)
		throw Server::ServerSocketException("createEpoll() failed");
}

void Server::createSocket(void) {
	this->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd == -1)
		throw Server::ServerSocketException("socket() failed");
	int value = 1;
	// SO_REUSEADDR = Permet de reeutiliser le port directement apres relancement du programme
	// pas besoin d'attendre 2-3mins
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
		throw Server::ServerSocketException("setsockopt() failed");
	// Modifier les proprietes du fd : on rend ca non bloquant (read, write, ...), pour que plusieurs clients
	// puissent interargir au meme moment sans que ca gele le thread
	if (fcntl(this->fd, F_SETFL, O_NONBLOCK) == -1)
		throw Server::ServerSocketException("fcntl() failed");

	struct sockaddr_in config;
	config.sin_family = AF_INET; //IPv4
	config.sin_port = htons(this->port);
	config.sin_addr.s_addr = INADDR_ANY; // Ecoute toutes les interfaces

	if (bind(this->fd, (struct sockaddr*)&config, sizeof(config)) == -1)
		throw Server::ServerSocketException(Utils::toString(this->port) + " is already used");
	if (listen(this->fd, SOMAXCONN) == -1) //Defini le nombre max de connection (4096 la)
		throw Server::ServerSocketException("listen() failed");
	
	this->epoll = epoll_create1(0);
	this->createEpoll(this->fd);
}

void Server::disconnect(int fd, std::string reason) {
	Client* client = this->clients.at(fd);
	std::map<std::string, Channel*> copy = this->channels;
	std::map<std::string, Channel*>::iterator it = copy.begin();
	while (it != copy.end()) {
		Channel* channel = this->getChannel(it->first);
		channel->leave(client, reason);
		it++;
	}

	delete client;
	this->clients.erase(fd);
	epoll_ctl(this->epoll, EPOLL_CTL_DEL, fd, NULL);
}

bool Server::isNickExist(std::string nick, Client* excludeClient) {
	std::map<int, Client*>::iterator it = this->clients.begin();
	while (it != this->clients.end()) {
		Client *curr = it->second;
		if (curr != excludeClient && curr->getNick() == nick) {
			return true;
		}
		it++;
	}
	return false;
}

void Server::broadcast(std::string message) {
	std::map<int, Client*>::iterator it = this->clients.begin();
	while (it != this->clients.end()) {
		it->second->sendMessage(message);
		it++;
	}
}

void Server::computeChannel(std::string name, Client *client, std::string password) {
	if (name[0] != '#')
		return;
	try {
		this->channels.at(name)->join(client, password);
	} catch (std::out_of_range &e) {
		this->channels[name] = new Channel(name, client);
	}
}

Channel *Server::getChannel(std::string name) {
	try {
		return this->channels.at(name);
	} catch (std::out_of_range &e) {
		return NULL;
	}
}

void Server::deleteChannel(Channel *channel) {
	if (this->getChannel(channel->getName()) == NULL)
		return;
	this->channels.erase(channel->getName());
	delete channel;
}

Client* Server::getClient(std::string nick) {
	std::map<int, Client*>::iterator it = this->clients.begin();
	while (it != this->clients.end()) {
		Client* client = it->second;
		if (client->getNick() == nick)
			return client;
		it++;
	}
	return NULL;
}

Client* Server::getClient(int fd) {
	try {
		return this->clients.at(fd);
	} catch (std::out_of_range &e) {
		return NULL;
	}
}

void Server::handleNewClient(void) {
	struct sockaddr_in ip;
	socklen_t size = sizeof(ip);

	int clientFd = accept(this->fd, (sockaddr *)(&ip), &size);
	if (clientFd == -1) {
		Logger::warning("Failed to accept client");
		return;
	}
	this->createEpoll(clientFd);
	this->clients[clientFd] = new Client(clientFd, inet_ntoa(ip.sin_addr));
}

void Server::handleMessage(int fd) {
	char buff[1024];

	ssize_t bytes = recv(fd, buff, sizeof(buff) , 0);

	if(bytes <= 0) {
		this->disconnect(fd, "Disconnected");
		return ;
	}
	Client* client = this->clients[fd];
	if (client == NULL)
		return;
	if (bytes > MAX_MESSAGE_SIZE) {
		client->sendError("Line too long");
		return ;
	}
	buff[bytes] = '\0';
	client->push(buff, bytes);
}

void Server::processLine(Client* client, std::string line) {
	if (client == NULL || client->getFd() == -1) {
		Logger::warning("Tried to process line with an invalid client");
		return;
	}
	//Command
	std::string command = line.substr(0, line.find(' '));
	line.erase(0, command.size() + 1);

	//Trailing
	std::string trailing;
	size_t trailingIndex = line.find(":");
	if (trailingIndex != std::string::npos) {
		trailing = line.substr(trailingIndex + 1, line.size() - trailingIndex);
		line.erase(trailingIndex, line.size());
	}

	//Params
	std::vector<std::string> params;
	size_t index;
	while (line.length() > 0) {
		index = line.find(' ');
		if (index == std::string::npos) {
			if (line.length() > 0)
				index = line.size();
			else
				break;
		}

		params.push_back(line.substr(0, index));
		line.erase(0, index + 1);
	}
	this->commandHandler->execute(client, command, params, trailing);
}

void Server::alert(Client *client, std::string message) {
	Logger::warning(message);
	client->sendError(message);
}

void Server::live(void) {
	epoll_event events[MAX_EVENTS];
	this->commandHandler = new CommandHandler();
	while (!Server::shouldClose){
		int size = epoll_wait(this->epoll, events, MAX_EVENTS, -1);
		if (size == -1) {
			if (Server::shouldClose)
				return;
			throw Server::ServerRuntimeException("epoll_wait() failed");
		}
		for (int i = 0; i < size; i++) {
			int fd = events[i].data.fd;
			if (fd == this->fd) // Si le serveur est prêt pour lire -> nouveau client
					this->handleNewClient();
				else // Sinon gérer le data
					this->handleMessage(fd);
			}
			
	}
	this->closeServer();
}

bool Server::isClosing() {
	return this->shouldClose;
}

std::string Server::getPassword() {
	return this->password;
}

Server* Server::get(void) {
	return Server::instance;
}