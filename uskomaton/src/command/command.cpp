#include "command/command.hpp"
using namespace uskomaton::command;
Command::Command(const std::string& name) : name(name) {

}

Command::~Command() {

}

const std::string& Command::getName() const {
	return name;
}