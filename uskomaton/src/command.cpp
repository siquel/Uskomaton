#include "command.hpp"
using namespace uskomaton;
Command::Command(const std::string& name) : name(name) {

}


PerlCallback::PerlCallback(const std::string& name) : Command(name) {

}

void PerlCallback::handleLine(Bot& bot, const std::string& channel, const std::string& sender) {

}