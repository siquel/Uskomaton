#pragma once
#include <string>
#include <vector>
#include <scripting.hpp>

namespace uskomaton {
	class Command;
	using namespace uskomaton::scripting;
	class Bot {
	public:
		Bot(const std::string& name, const std::string& login);
		~Bot();
		void initialize();
		void addCommand(Command* command);
	private:
		bool initialized;
		std::string name;
		std::string login;
		std::vector<uskomaton::Command*> commands;
		std::vector<ScriptingAPI*> script;
	};
}