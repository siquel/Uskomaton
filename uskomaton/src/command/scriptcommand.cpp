#include "command.hpp"
#include "bot.hpp"
using namespace uskomaton::command;
void ScriptCommand::handleLine(uskomaton::Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) {

}

ScriptCommand::ScriptCommand(const std::string& name) 
	: Command(name) {

}

ScriptCommand::~ScriptCommand() {

}
