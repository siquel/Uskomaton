#include "bot.hpp"
#include "perl.hpp"
#include "command.hpp"
#include <algorithm>
using namespace uskomaton;
Bot::Bot(const uskomaton::config::Configuration& conf)
	: initialized(false), listener(this), config(conf) {
		script.push_back(PerlScriptingAPI::getInstance());
		//client.addListener(&listener);
}

Bot::~Bot() {
	for (size_t i = 0; i < script.size(); i++) {
		delete script[i];
		script[i] = nullptr;
	}
	
	for (size_t i = 0; i < commands.size(); i++) {
		delete commands[i];
		commands[i] = nullptr;
	}
	
}

void Bot::initialize() {
	if (initialized) return;
	commands.reserve(10);
	for (size_t i = 0; i < script.size(); i++) {
		script[i]->initialize(this);
		const std::string& autoload = config.getAutoloadPath();
		if (!autoload.empty()) {
			script[i]->autoloadFrom(autoload);
		}
	}

	for (auto& serverConfig : config.getServerConfigs()) {
		IrcClient* client = new IrcClient(serverConfig.username, serverConfig.login);
		client->addListener(&listener);
		clients.push_back(client);
		
	}

	initialized = true;
}

void Bot::addCommand(uskomaton::Command* command) {
	commands.push_back(command);
}

const std::vector<ScriptingAPI*>& Bot::getScripts() const {
	return script;
}

void Bot::sendMessage(const std::string& channel, const std::string& message) {
	std::cout << channel << " >> " << message << std::endl;
	//client.sendMessage(channel, message);
}

void uskomaton::Bot::joinChannel(const std::string& channel) {
	//client.sendJoinChannel(channel);
}

void uskomaton::Bot::terminate() {
	for (size_t i = 0; i < clients.size(); i++){
		clients[i]->terminate();
	}
}

void uskomaton::Bot::connectServers() {
	size_t i = 0;
	for (auto& serverConfig : config.getServerConfigs()) {
		clients[i]->connectTo(serverConfig.hostname, serverConfig.port);
		for (auto& channel : serverConfig.channels) {
			clients[i]->sendJoinChannel(channel);
		}
		i++;
	}
}

