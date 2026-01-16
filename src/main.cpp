#include "Server.hpp"

int main (int argc, char **argv) {
	if (argc != 3) {
		std::cout << "./ircserv <port> <password>" << std::endl;
		return (0);
	}

	std::istringstream iss(argv[1]);
	uint16_t port;
	iss >> std::noskipws >> port;
	if (!iss.eof() || iss.fail() || !std::isdigit(argv[1][0])) {
		std::cerr << "Error: not a valid port" << std::endl;
		return (0);
	}
	std::string password = argv[2];
	if (password.length() < MIN_PASSWORD || password.length() > MAX_PASSWORD) {
		std::cerr << "Error: password should be " << MIN_PASSWORD << " to " << MAX_PASSWORD << " character long" << std::endl;
		return (0);
	}

	#ifdef DEBUG
		Logger::debug("Mode Debug");
	#endif
	Server server;

	try {
		server.start(port, password);
	} catch (const std::exception& e) {
		Logger::error("An error has occured : ", false);
		std::cerr << e.what() << std::endl;
	}
}