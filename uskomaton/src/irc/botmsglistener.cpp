#include "irc/botmsglistener.hpp"

#include "bot.hpp"
#include <algorithm>
BotMessageListener::BotMessageListener(uskomaton::Bot* bot, const uskomaton::config::ServerConfiguration& config)
	: bot(bot), config(config) {

}

void BotMessageListener::onRawMessage(const std::string& raw, const std::string& command, const std::string& target) {
	/*
	std::string& context = config.name;
	std::for_each(bot->getScripts().begin(), bot->getScripts().end(), [&raw, &command, &target, &context](uskomaton::scripting::ScriptingAPI* api) {
		api->processRawMessage(raw, context, command, target);
	});*/
}

void BotMessageListener::onMessage(const std::string& channel, const std::string& message, const std::string& sender) {
	std::string& context = config.name;
	std::for_each(bot->getScripts().begin(), bot->getScripts().end(), [&channel, &message, &sender, &context](uskomaton::scripting::ScriptingAPI* api) {
		api->processOnMessage(context, channel, message, sender);
	});
}