#include "script/perl.hpp"
#include "command/perl_script_command.hpp"
using namespace uskomaton::command;

PerlScriptCommand::PerlScriptCommand(const HookData* data) : ScriptCommand(data->name), data(data) {

}

void PerlScriptCommand::handleLine(uskomaton::Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) {
	using namespace uskomaton::scripting;
	PerlScriptingAPI::getInstance()->executeHookWithArguments(data, 4, context.c_str(), channel.c_str(), message.c_str(), sender.c_str());
}