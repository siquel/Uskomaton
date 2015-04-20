#pragma once
#include <string>
#include "forward_declare.hpp"
USKOMATON_FORWARD_DECL_1(uskomaton, Bot);
namespace uskomaton {
	namespace command {

		class CommandInterface {
		public:
			CommandInterface(uskomaton::Bot* bot, const std::string& prefix);
			~CommandInterface() = default;
			void processLine(const std::string& context, const std::string& channel, const std::string& message, const std::string& sender);
		private:
			uskomaton::Bot* bot;
			const std::string prefix;
		};
	}
}
