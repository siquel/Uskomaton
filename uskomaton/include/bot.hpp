#pragma once
#include <string>
#include <vector>
#include <scripting.hpp>
#include "irc/botmsglistener.hpp"
namespace uskomaton {
	class Command;
	using namespace uskomaton::scripting;
	class Bot {
	public:
		Bot(const std::string& name, const std::string& login);
		~Bot();
		void initialize();
		void addCommand(Command* command);
		const std::vector<ScriptingAPI*>& getScripts() const;
		void connectTo(const std::string& hostname);
		void sendMessage(const std::string& channel, const std::string& message);
		// debug
		void start();
	private:
		bool initialized;
		IrcClient client;
		BotMessageListener listener;
		std::vector<uskomaton::Command*> commands;
		std::vector<ScriptingAPI*> script;
	};
}