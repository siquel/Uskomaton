#include "irc/botmsglistener.hpp"
#include "bot.hpp"
#include <algorithm>
BotMessageListener::BotMessageListener(uskomaton::Bot* bot) : bot(bot) {

}

void BotMessageListener::onRawMessage(const std::string& raw, const std::string& command, const std::string& target) {
	std::for_each(bot->getScripts().begin(), bot->getScripts().end(), [&raw, &command, &target](uskomaton::scripting::ScriptingAPI* api) {
		api->processRawMessage(raw, command, target);
	});
}