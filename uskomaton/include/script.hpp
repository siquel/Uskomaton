#pragma once
#include <string>
#include <vector>
#include "command.hpp"
namespace uskomaton {
	namespace command {
		class ScriptCommand;
		namespace hook {
			class ServerHook {
			public:
				ServerHook(const std::string& message);
				const std::string& getMessage() const {
					return message;
				}
			private:
				const std::string message;
			};
		}
	}
	namespace scripting {
		class Script {
		public:
			Script(const std::string& filename, const std::string& name);
			~Script();
			const std::string& getFilename() const;
			const std::string& getName() const;
			void hookCommand(command::ScriptCommand* command);
			void hookServer(command::hook::ServerHook*);
			command::ScriptCommand* getCommandByName(const std::string& name) const;
			const std::vector<command::hook::ServerHook*>& getServerHooks() const;
		private:
			const std::string filename;
			const std::string name;
			std::vector<command::ScriptCommand*> commands;
			std::vector<command::hook::ServerHook*> serverHooks;
		};
	}
}