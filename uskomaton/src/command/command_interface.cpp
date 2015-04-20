#include "command/command_interface.hpp"
#include "bot.hpp"
#include <script/script.hpp>
#include "script/scripting.hpp"
#include "command/script_command.hpp"
#include "boost/algorithm/string.hpp"
using namespace uskomaton::command;

CommandInterface::CommandInterface(uskomaton::Bot* bot, const std::string& prefix)
	: bot(bot), prefix(prefix) {

}

void CommandInterface::processLine(const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) {
	if (!boost::starts_with(message, prefix)) return;
	std::string parsedMessage(message.substr(prefix.size()));
	size_t space = 0;
	if ((space = parsedMessage.find(" ")) == std::string::npos) {
		space = parsedMessage.size();
	}
	std::string name(parsedMessage.substr(0, space));
	parsedMessage = parsedMessage.substr(name.size() + (parsedMessage.find(" ") != std::string::npos ? 1 : 0));

	auto commands = bot->getCommands();
	for (size_t i = 0; i < commands.size(); i++) {
		if (commands[i]->getName() != name) continue;

		commands[i]->handleLine(*bot, context, channel, parsedMessage, sender);
		return;
	}

	using namespace uskomaton::scripting;
	for (size_t i = 0; i < bot->getScripts().size(); i++) {
		ScriptingAPI* api = bot->getScripts()[i];
		for (size_t j = 0; j < api->getScripts().size(); j++) {
			Script* script = api->getScripts()[j];
			ScriptCommand* command = script->getCommandByName(name);
			// not found
			if (command == nullptr) continue;
			command->handleLine(*bot, context, channel, parsedMessage, sender);
			return;
		}
	}
}
