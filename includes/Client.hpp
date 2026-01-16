#pragma once
#include "Server.hpp"

#define EOL "\r\n"
#define MAX_NICK_LENGTH 30


enum IrcCode
{
	RPL_WELCOME = 001,
	RPL_WHOISUSER = 311,
	RPL_ENDOFWHOIS = 318,
	RPL_CHANNELMODEIS = 324,
	RPL_TOPIC = 332,
	RPL_INVITING = 341,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,

	ERR_NICKNAMEINUSE = 433,

	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL  = 403,
	ERR_USERNOTINCHANNEL = 441,
    ERR_NOTONCHANNEL   = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_PASSWDMISMATCH = 464,
	ERR_CHANNELISFULL = 471,
	ERR_UNKNOWNMODE = 472,
	ERR_INVITEONLYCHAN = 473,
	ERR_BADCHANNELKEY = 475,
	ERR_CHANOPRIVSNEEDED = 482
};

class Client
{
	private:
		int fd;
		std::string ip;
		std::string buffer;
		std::string nick;
		std::string username;
		std::string realname;
		bool registered;
		bool connected;

	public:
		Client(int fd, std::string ip);
		~Client();
		void connect();
		void disconnect();
		void push(char *buff, ssize_t bytes);
		void sendServerMessage(std::string content);
		void sendMessage(std::string content);
		void sendError(std::string error);
		void sendNumeric(IrcCode code, std::string message);
		int getFd();
		std::string getIp();
		std::string getNick();
		void setNick(std::string nick);
		std::string getUsername();
		std::string getRealName();
		void setName(std::string usermame, std::string realname);
		bool isConnected();
		void setRegistered();
		bool isRegistered();
		std::string formatName();
};