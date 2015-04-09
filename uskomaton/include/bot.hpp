#pragma once
#include <string>
#include <vector>
#include <scripting.hpp>
#include "irc/botmsglistener.hpp"
#include "conf/config.hpp"
namespace uskomaton {
	class Command;
	using namespace uskomaton::scripting;
	class Bot {
	public:
		Bot(const uskomaton::config::Configuration& conf);
		~Bot();
		void initialize();
		void addCommand(Command* command);
		const std::vector<ScriptingAPI*>& getScripts() const;
		void connectTo(const std::string& hostname);
		void connectServers();
		void sendMessage(const std::string& channel, const std::string& message);
		void joinChannel(const std::string& channel);
		// debug
		void start();
		void terminate();
	private:
		uskomaton::config::Configuration config;
		bool initialized;
		std::vector<IrcClient*> clients;
		BotMessageListener listener;
		std::vector<uskomaton::Command*> commands;
		std::vector<ScriptingAPI*> script;
	};
}