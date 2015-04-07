#include <iostream>
#include "irc/ircclient.hpp"
#include <algorithm>
#include "util.hpp"

void IrcMessageListener::onRawMessage(const std::string& msg, const std::string& command, const std::string& target) {}
void IrcMessageListener::onServerPing(const std::string& ping) {}

IrcClient::IrcClient(const std::string& nick, const std::string& login) : nick(nick), login(login){

}

void IrcClient::addListener(IrcMessageListener* listener) {
	listeners.push_back(listener);
}

void IrcClient::notifyListeners(notification_t what) {
	std::for_each(listeners.begin(), listeners.end(), what);
}

void IrcClient::connectTo(const std::string& hostname, int port, const std::string& password) {
	sendNick(nick);
	sendUser(login);
	sendJoinChannel("#test123");
	sendJoinChannel("#testpass", "password");
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

	if (raw.empty()) return;

	std::vector<std::string> tokens = uskomaton::util::split(raw, ' ');
	if (tokens.at(0).at(0) == ':') {
		tokens.erase(tokens.begin());
	}

	std::string command = tokens[0];
	tokens.erase(tokens.begin());

	if (command == "PING") {
		onServerPing(tokens[0]);
		// gotta notify scripts 
		notifyListeners([&raw, &command](IrcMessageListener* listener) {
			listener->onRawMessage(raw, command, "");
		});
		return;
	}

	std::string target = (tokens.empty() ? "" : tokens[0]);
	if (target.at(0) == ':') {
		target = target.substr(1);
	}

	// valid IRC line?
	if (raw.at(0) != ':') {
		// TODO unknown line
		return;
	}
	
	// TODO parse users etc

	notifyListeners([&raw, &command, &target](IrcMessageListener* listener) {
		listener->onRawMessage(raw, command, target);
	});
}

void IrcClient::onServerPing(const std::string& ping) {
	// TODO respond ping
	notifyListeners([&ping](IrcMessageListener* listener) {
		listener->onServerPing(ping);
	});
}

void IrcClient::sendMessage(const std::string& channel, const std::string& message) {
	std::string line = std::string("PRIVMSG ") + channel + std::string(" :") + message;
	sendRawMessage(line);
}

void IrcClient::sendRawMessage(const std::string& line) {
	std::cout << line.c_str() << "\r\n";
}

void IrcClient::sendNick(const std::string& nick) {
	sendRawMessage(std::string("NICK ") + nick);
}

void IrcClient::sendUser(const std::string& login) {
	// TODO mode?
	sendRawMessage(std::string("USER ") + login + std::string(" 0 * :Real name"));
}

void IrcClient::sendJoinChannel(const std::string& channel, const std::string& password) {
	sendRawMessage(std::string("JOIN ") + channel + (password.empty() ? std::string("") : std::string(" ") + password));
}

void IrcClient::sendPartChannel(const std::string& channel) {
	sendRawMessage(std::string("PART ") + channel);
}