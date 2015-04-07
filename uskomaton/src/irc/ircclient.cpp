#include "irc/ircclient.hpp"
#include <algorithm>

void IrcMessageListener::onRawMessage(const std::string& msg) {}

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