#pragma once

#include "Server.hpp"

class Client;

class Command {
	private:
		std::string name;
		size_t minParams;
		size_t maxParams;
		bool trailingRequired;
		bool connected;
	public:
		Command(std::string name, size_t minParams, size_t maxParams, bool trailingRequired) {
			this->name = name;
			this->minParams = minParams;
			this->maxParams = maxParams;
			this->trailingRequired = trailingRequired;
			this->connected = true;
		}
		Command(std::string name, size_t minParams, size_t maxParams, bool trailingRequired, bool connected) {
			this->name = name;
			this->minParams = minParams;
			this->maxParams = maxParams;
			this->trailingRequired = trailingRequired;
			this->connected = connected;
		}
		virtual ~Command(){};
		virtual void execute(Client *client, std::vector<std::string> params, std::string trailing) = 0;

		bool canExecuted(std::vector<std::string> params, std::string trailing) {
			if (params.size() < minParams || params.size() > maxParams)
				return false;
			if (trailingRequired && trailing.size() == 0)
				return false;
			return true;
		}

		std::string getName() {
			return this->name;
		}

		int getMinParams() {
			return this->minParams;
		}

		bool isTrailingRequired() {
			return this->trailingRequired;
		}

		bool mustBeConnected() {
			return this->connected;
		}
};