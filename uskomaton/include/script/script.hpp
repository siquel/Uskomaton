#pragma once
#include <string>
#include <vector>
#include "forward_declare.hpp"
USKOMATON_FORWARD_DECL_2(uskomaton, command, ScriptCommand);
USKOMATON_FORWARD_DECL_2(uskomaton, scripting, ServerHook);
namespace uskomaton {
	namespace scripting {
		class Script {
		public:
			Script(const std::string& filename, const std::string& name);
			~Script();
			const std::string& getFilename() const;
			const std::string& getName() const;
			void hookCommand(command::ScriptCommand* command);
			void hookServer(ServerHook*);
			command::ScriptCommand* getCommandByName(const std::string& name) const;
			const std::vector<ServerHook*>& getServerHooks() const;
		private:
			const std::string filename;
			const std::string name;
			std::vector<command::ScriptCommand*> commands;
			std::vector<ServerHook*> serverHooks;
		};
	}
}