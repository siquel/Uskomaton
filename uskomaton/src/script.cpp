#include "script.hpp"
#include "command.hpp"
using namespace uskomaton::scripting;

Script::Script(const std::string& filename, const std::string& name) 
	: filename(filename), name(name) {

}

Script::~Script() {

}

const std::string& Script::getFilename() const {
	return filename;
}

const std::string& Script::getName() const {
	return name;
}

void uskomaton::scripting::Script::hookCommand(command::ScriptCommand* command) {
	commands.push_back(command);
}

uskomaton::command::ScriptCommand* uskomaton::scripting::Script::getCommandByName(const std::string& name) const {
	for (size_t i = 0; i < commands.size(); i++) {
		if (commands[i]->getName() == name) return commands[i];
	}
	return nullptr;
}
