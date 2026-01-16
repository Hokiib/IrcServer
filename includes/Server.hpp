#pragma once
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
#include <exception>
#include "Logger.hpp"
#include "Utils.hpp"
#include <csignal>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include "Client.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "CommandHandler.hpp"
#include "Channel.hpp"

# define MIN_PASSWORD 3
# define MAX_PASSWORD 16
# define MAX_MESSAGE_SIZE 512
# define MAX_EVENTS 10
# define SERVER_NAME "server"


class Client;
class CommandHandler;
class Channel;

class Server {
	private:
		static Server* instance;
		uint16_t port;
		std::string password;
		CommandHandler* commandHandler;
		int fd;
		int epoll;
		bool closed;
		std::map<int, Client*> clients;
		std::map<std::string, Channel*> channels;
		static bool shouldClose;

		void createEpoll(int fd);
		void createSocket(void);
		void handleNewClient();
		void handleMessage(int fd);
		static void handleSignal(int s);
	public:
		Server();
		~Server();
		static Server* get(void);
		void disconnect(int fd, std::string reason);
		void alert(Client *client, std::string message);
		void processLine(Client* client, std::string line);
		void start(uint16_t port, std::string password);
		void live(void);
		void closeServer(void);
		void broadcast(std::string message);
		bool isNickExist(std::string nick, Client* excludeClient);
		void computeChannel(std::string name, Client* client, std::string password);
		Channel *getChannel(std::string name);
		Client *getClient(std::string nick);
		Client* getClient(int fd);
		void deleteChannel(Channel *channel);
		std::string getPassword();
		bool isClosing();

	class ServerException : public std::exception {
		private:
			std::string _msg;
		public:
		 	ServerException(std::string exception, std::string error) {
				this->_msg = exception + ": " + error;
			}
			virtual ~ServerException() throw() {}
			virtual const char *what() const throw() {
				return this->_msg.c_str();
			}
	};
	class ServerRuntimeException : public ServerException
	{
		public:
			ServerRuntimeException(std::string error): ServerException("ServerRuntimeException", error){};
	};
	class ServerSocketException : public ServerException
	{
		public:
			ServerSocketException(std::string error): ServerException("ServerSocketException", error){};
	};
};