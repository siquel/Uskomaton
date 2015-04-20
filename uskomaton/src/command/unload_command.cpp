#include "command/unload_command.hpp"
#include "bot.hpp"
#include "script/script.hpp"
#include "script/scripting.hpp"
using namespace uskomaton::command;
UnloadCommand::UnloadCommand() : Command("unload") {

}

void UnloadCommand::handleLine(uskomaton::Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) {
	
	for (size_t i = 0; i < bot.getScripts().size(); i++) {
		ScriptingAPI* api = bot.getScripts()[i];
		Script* script = nullptr;
		script = api->getScript([&message](Script* s){
			return s->getName() == message;
		});

		if (script == nullptr) continue;

		bot.sendMessage(context, channel, script->getName() + script->getFilename());
		api->unload(script);
		return;
	}
}


