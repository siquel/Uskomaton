#pragma once
#include "command/script_command.hpp"
struct HookData;
namespace uskomaton {
	namespace command {
		class ScriptCommand;
		class PerlScriptCommand : public ScriptCommand {
		public:
			PerlScriptCommand(const HookData* data);
			void handleLine(uskomaton::Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender);
		private:
			const HookData* data;
		};
	}
}