#include "bot.hpp"
#include "perl.hpp"
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
	std::for_each(script.begin(), script.end(), [](ScriptingAPI* api) {
		api->initialize();
	});
	initialized = true;
}