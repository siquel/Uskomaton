#include <iostream>
#include "irc/ircclient.hpp"
#include <algorithm>

void IrcMessageListener::onRawMessage(const std::string& msg) {}

IrcClient::IrcClient(const std::string& nick, const std::string& login) : nick(nick), login(login){

}

void IrcClient::addListener(IrcMessageListener* listener) {
	listeners.push_back(listener);
}

void IrcClient::notifyListeners(notification_t what) {
	std::for_each(listeners.begin(), listeners.end(), what);
}

void IrcClient::connectTo(const std::string& hostname, int port, const std::string& password) {

}
void IrcClient::connectTo(const std::string& hostname, int port) {
	connectTo(hostname, port, "");
}
void IrcClient::connectTo(const std::string& hostname) {
	connectTo(hostname, 6667, "");
}

void IrcClient::start() {
	size_t i = 0;
	while (i++ < 10) {
		onRawMessage(":Macha!~macha@unaffiliated/macha PRIVMSG #botwar :Test response\r\n");
	}
}

void IrcClient::onRawMessage(const std::string& raw) {
	notifyListeners([&raw](IrcMessageListener* listener) {
		listener->onRawMessage(raw);
	});
}

void IrcClient::sendMessage(const std::string& channel, const std::string& message) {
	std::string line = std::string("PRIVMSG ") + channel + std::string(" :") + message;
	sendRawMessage(line);
}

void IrcClient::sendRawMessage(const std::string& line) {
	std::cout << line.c_str() << "\r\n";
}