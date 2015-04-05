#pragma once
#include <string>
#include <vector>
#include <scripting.hpp>

namespace uskomaton {
	using namespace uskomaton::scripting;
	class Bot {
	public:
		Bot(const std::string& name, const std::string& login);
		~Bot();
		void initialize();
	private:
		bool initialized;
		std::string name;
		std::string login;
		std::vector<ScriptingAPI*> script;
	};
}