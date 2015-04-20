#include "irc/channel.hpp"
using namespace uskomaton::irc;

Channel::Channel(const std::string& name)
	: name(name) {

}

const std::string& uskomaton::irc::Channel::getTopic() const {
	return topic;
}

const std::string& uskomaton::irc::Channel::getName() const {
	return name;
}

const void uskomaton::irc::Channel::getNicks(std::vector<std::string>& nicks) const {

}
