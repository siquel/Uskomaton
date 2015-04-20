#pragma once
#include <string>
#include <vector>
#include <map>
#include "irc/botmsglistener.hpp"
#include "irc/channel.hpp"
#include "conf/config.hpp"
#include "forward_declare.hpp"
USKOMATON_FORWARD_DECL_2(uskomaton, command, Command);
USKOMATON_FORWARD_DECL_2(uskomaton, scripting, ScriptingAPI);
namespace uskomaton {
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
		bool initialized;
		uskomaton::config::Configuration config;
		IrcClient* getClientFor(const std::string& name) const;
		std::vector<IrcClient*> clients;
		std::map<std::string, std::vector<Channel>> channels;
		std::vector<command::Command*> commands;
		std::vector<ScriptingAPI*> script;
	};
}