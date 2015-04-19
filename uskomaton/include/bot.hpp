#pragma once
#include <string>
#include <vector>
#include <map>
#include <scripting.hpp>
#include "irc/botmsglistener.hpp"
#include "irc/channel.hpp"
#include "conf/config.hpp"

namespace uskomaton {
	namespace command {
		class Command;
	}
	using namespace uskomaton::scripting;
	using namespace uskomaton::irc;
	class Bot {
	public:
		Bot(const uskomaton::config::Configuration& conf);
		~Bot();
		void initialize();
		void addCommand(command::Command* command);
		const std::vector<ScriptingAPI*>& getScripts() const;
		void connectServers();
		void sendMessage(const std::string& context, const std::string& channel, const std::string& message);
		void joinChannel(const std::string& context, const std::string& channel);
		const uskomaton::config::ServerConfiguration* getConfigFor(const std::string& name) const;
		void terminate();
		const std::vector<Channel>& getJoinedChannels(const std::string& serverName);
		const std::vector<uskomaton::command::Command*> getCommands() const;
	private:
		uskomaton::config::Configuration config;
		IrcClient* getClientFor(const std::string& name) const;
		bool initialized;
		std::vector<IrcClient*> clients;
		std::map<std::string, std::vector<Channel>> channels;
		std::vector<command::Command*> commands;
		std::vector<ScriptingAPI*> script;
	};
}