#pragma once
#include "command.hpp"
namespace uskomaton {
	namespace command {
		class ScriptCommand : public Command {
		public:
			ScriptCommand(const std::string& name);
			~ScriptCommand();
			void handleLine(Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) override;
		private:

		};
	}
}
