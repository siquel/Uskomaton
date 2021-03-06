#pragma once
#include "command.hpp"
#include "forward_declare.hpp"
USKOMATON_FORWARD_DECL_1(uskomaton, Bot);
namespace uskomaton {
	namespace command {
		class UnloadCommand : public Command {
		public:
			UnloadCommand();
			void handleLine(Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) override;
		};
	}
}