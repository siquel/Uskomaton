#include "command/commandinterface.hpp"
#include "bot.hpp"
#include <script.hpp>
#include "scripting.hpp"
#include "boost/algorithm/string.hpp"
using namespace uskomaton::command;

CommandInterface::CommandInterface(uskomaton::Bot* bot, const std::string& prefix)
	: bot(bot), prefix(prefix) {

}

void CommandInterface::processLine(const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) {
	if (!boost::starts_with(message, prefix)) return;
	std::string parsedMessage(message.substr(prefix.size()));
	int space = 0;
	if ((space = parsedMessage.find(" ")) == std::string::npos) {
		space = parsedMessage.size();
	}
	std::string name(parsedMessage.substr(0, space));
	using namespace uskomaton::scripting;
	for (size_t i = 0; i < bot->getScripts().size(); i++) {
		ScriptingAPI* api = bot->getScripts()[i];
		for (size_t j = 0; j < api->getScripts().size(); j++) {
			Script* script = api->getScripts()[j];
			ScriptCommand* command = script->getCommandByName(name);
			// not found
			if (command == nullptr) continue;
			command->handleLine(*bot, context, channel, message, sender);
		}
	}
}
