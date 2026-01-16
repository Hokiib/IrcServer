#pragma once

#include "Utils.hpp"

# define RED "\e[1;31m"
# define GREEN "\e[1;32m"
# define BLUE "\e[1;34m"
# define YELLOW "\e[1;33m"
# define PURPLE "\e[1;35m"

# define ITALIC "\e[3m"
# define RESET "\e[1;0m"

class Logger {
	public:
		static void error(std::string message, bool newline) {
			std::ostream &stream = std::cerr << RED << message << RESET;
			if (newline)
				stream << std::endl;
		}
		static void error(std::string message) {
			Logger::error(message, true);
		}
		static void success(std::string message, bool newline) {
			std::ostream &stream = std::cout << GREEN << message << RESET;
			if (newline)
				stream << std::endl;
		}
		static void success(std::string message) {
			Logger::success(message, true);
		}
		static void info(std::string message, bool newline) {
			std::ostream &stream = std::cout << message << RESET;
			if (newline)
				stream << std::endl;
		}
		static void info(std::string message) {
			Logger::info(message, true);
		}
		static void warning(std::string message, bool newline) {
			std::ostream &stream = std::cout << YELLOW << message << RESET;
			if (newline)
				stream << std::endl;
		}
		static void warning(std::string message) {
			Logger::warning(message, true);
		}
		static void debug(std::string message, bool newline) {
			#ifndef DEBUG
				return;
			#endif
				
			std::ostream &stream = std::cout << ITALIC PURPLE << message << RESET;
			if (newline)
				stream << std::endl;
		}
		static void debug(std::string message) {
			Logger::debug(message, true);
		}
		template <typename Any>
		static void debug(Any content) {
			Logger::debug(Utils::toString(content), true);
		}
};