#include "bot.hpp"
#include "perl.hpp"
#include "command.hpp"
#include <algorithm>
using namespace uskomaton;
Bot::Bot(const std::string& name, const std::string& login) 
	: name(name), login(login), initialized(false) {
		script.push_back(PerlScriptingAPI::getInstance());
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