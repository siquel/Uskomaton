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
		void sendMessage(const std::string& context, const std::string& channel, const std::string& message);
		void joinChannel(const std::string& context, const std::string& channel);
		const uskomaton::config::ServerConfiguration* getConfigFor(const std::string& name) const;
		// debug
		void start();
		void terminate();
	private:
		uskomaton::config::Configuration config;
		IrcClient* getClientFor(const std::string& name) const;
		bool initialized;
		std::vector<IrcClient*> clients;
		std::vector<uskomaton::Command*> commands;
		std::vector<ScriptingAPI*> script;
	};
}