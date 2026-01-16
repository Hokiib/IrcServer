#pragma once
#include "Server.hpp"

typedef std::vector<Client *>::iterator iterator;

class Channel {
	private:
		std::string name;
		std::vector<Client *> operators;
		std::vector<Client *> users;
		std::vector<Client *> invited;
		std::string password;
		std::string topic;
		size_t limit;
		bool inviteOnly;
		bool topicOperator;
	public:
		Channel(std::string name, Client *owner);
		~Channel();
		void join(Client *client, std::string password);
		void leave(Client *client, std::string message);
		void kick(Client *client, Client* by, std::string reason);

		void broadcast(std::string message);
		void broadcastMode(std::string from, std::string message);
		void sendMessage(Client* client, std::string message);

		std::string getName();

		void setTopic(std::string topic, Client* by);
		std::string getTopic();

		void setUserLimit(size_t limit);
		size_t getUserLimit();

		void setInviteOnly(bool inviteOnly);
		bool isInviteOnly();
		void clearInvitation(Client* client);
		void invite(Client* client, Client* by);

		void setOp(Client *client, std::string by, bool op);
		bool isOp(Client *client);
		bool isUser(Client *client);
		bool isInvited(Client *client);

		bool isTopicOperator();
		void setTopicOperator(bool topicOperator);

		bool isFull();

		void setPassword(std::string password);
		std::string getPassword();

	private:
		void removeClient(Client *client);
		iterator getUser(Client *client);
		iterator getOperator(Client *client);
		iterator getInvited(Client *client);
};