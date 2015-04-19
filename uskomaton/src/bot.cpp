#include "bot.hpp"
#include "perl.hpp"
#include "command.hpp"
#include <algorithm>
#include "command/unload_command.hpp"
using namespace uskomaton;
Bot::Bot(const uskomaton::config::Configuration& conf)
	: initialized(false), config(conf) {
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
	using namespace uskomaton::command;
	commands.reserve(10);
	commands.push_back(new UnloadCommand());
	for (auto& serverConfig : config.getServerConfigs()) {
		IrcClient* client = new IrcClient(serverConfig.name, serverConfig.username, serverConfig.login);
		client->addListener(new BotMessageListener(this, serverConfig));
		clients.push_back(client);

	}

	for (size_t i = 0; i < script.size(); i++) {
		script[i]->initialize(this);
		const std::string& autoload = config.getAutoloadPath();
		if (!autoload.empty()) {
			script[i]->autoloadFrom(autoload);
		}
	}

	initialized = true;
}

void Bot::addCommand(uskomaton::command::Command* command) {
	commands.push_back(command);
}

const std::vector<ScriptingAPI*>& Bot::getScripts() const {
	return script;
}

const uskomaton::config::ServerConfiguration* Bot::getConfigFor(const std::string& name) const {
	using namespace uskomaton::config;
	auto configs = this->config.getServerConfigs();
	for (size_t i = 0; i < configs.size(); i++) {
		if (configs[i].name == name) return &configs[i];
	}
	return nullptr;
}

IrcClient* Bot::getClientFor(const std::string& name) const {
	using namespace uskomaton::config;
	
	for (size_t i = 0; i < clients.size(); i++) {
		IrcClient* client = clients[i];
		if (client->getServerName() == name) {
			return client;
		}
	}
	return nullptr;
}

void Bot::sendMessage(const std::string& context, const std::string& channel, const std::string& message) {
	std::cout << context << " " <<  channel << " >> " << message << std::endl;
	IrcClient* client = getClientFor(context);
	assert(client != nullptr);
	client->sendMessage(channel, message);
}

void uskomaton::Bot::joinChannel(const std::string& context, const std::string& channel) {
	IrcClient* client = getClientFor(context);
	assert(client != nullptr);
	client->sendJoinChannel(channel);
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

const std::vector<uskomaton::command::Command*> uskomaton::Bot::getCommands() const {
	return commands;
}

