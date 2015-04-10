#include "script.hpp"

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