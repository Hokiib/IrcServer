#pragma once

class Utils {
	public:
		template <typename T>
		static std::string toString(T any) {
			std::stringstream ss;
			ss << any;
			if (ss.fail())
				return NULL;
			return ss.str();
		} 
};
