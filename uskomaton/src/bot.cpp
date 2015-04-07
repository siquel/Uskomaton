#include "bot.hpp"
#include "perl.hpp"
#include "command.hpp"
#include <algorithm>
using namespace uskomaton;
Bot::Bot(const std::string& name, const std::string& login) 
	: client(name, login), initialized(false), listener(this) {
		script.push_back(PerlScriptingAPI::getInstance());
		client.addListener(&listener);
}

Bot::~Bot() {
	for (size_t i = 0; i < script.size(); i++) {
		delete script[i];
		script[i] = nullptr;
	}
	script.clear();
}

void Bot::initialize() {
	if (initialized) return;
	commands.reserve(10);
	for (size_t i = 0; i < script.size(); i++) {
		script[i]->initialize(this);
	}
	initialized = true;
}

void Bot::addCommand(uskomaton::Command* command) {
	commands.push_back(command);
}

const std::vector<ScriptingAPI*>& Bot::getScripts() const {
	return script;
}

void Bot::connectTo(const std::string& hostname) {
	client.connectTo(hostname);
}

void Bot::start() {
	client.start();
}

void Bot::sendMessage(const std::string& channel, const std::string& message) {
	client.sendMessage(channel, message);
}