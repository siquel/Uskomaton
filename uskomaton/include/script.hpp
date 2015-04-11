#pragma once
#include <string>
#include <vector>
#include "command.hpp"
namespace uskomaton {
	namespace command {
		class ScriptCommand;
	}
	namespace scripting {
		class Script {
		public:
			Script(const std::string& filename, const std::string& name);
			~Script();
			const std::string& getFilename() const;
			const std::string& getName() const;
			void hookCommand(command::ScriptCommand* command);
			command::ScriptCommand* getCommandByName(const std::string& name) const;
		private:
			const std::string filename;
			const std::string name;
			std::vector<command::ScriptCommand*> commands;
		};
	}
}