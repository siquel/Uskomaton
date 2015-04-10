#include <iostream>
#include "irc/ircclient.hpp"
#include <algorithm>
#include "util.hpp"

void IrcMessageListener::onRawMessage(const std::string& msg, const std::string& command, const std::string& target) {}
void IrcMessageListener::onMessage(const std::string& channel, const std::string& message, const std::string& sender) {}
void IrcMessageListener::onServerPing(const std::string& ping) {}

using namespace uskomaton::irc;

IrcClient::IrcClient(const std::string& context, const std::string& nick, const std::string& login)
	: server(context), nick(nick), login(login), socket(ioService){

}

IrcClient::~IrcClient() {
	//terminate();
}

void IrcClient::addListener(IrcMessageListener* listener) {
	listeners.push_back(listener);
}

void IrcClient::notifyListeners(notification_t what) {
	std::for_each(listeners.begin(), listeners.end(), what);
}

void IrcClient::connectTo(const std::string& hostname, int _port, const std::string& password) {
	using namespace boost::asio::ip;
	std::stringstream ss;
	ss << _port;
	std::string port;
	ss >> port;

	bool connected(false);
	try {
		tcp::resolver resolver(ioService);
		tcp::resolver::query query(hostname, port);
		tcp::resolver::iterator endpointIt = resolver.resolve(query);
		boost::asio::connect(socket, endpointIt);

		sendNick(nick);
		sendUser(login);
		while (true) {
			boost::asio::read_until(socket, buffer, "\r\n");
			std::istream is(&buffer);
			std::string line;
			std::getline(is, line);
			std::cout << line << std::endl;
			onRawMessage(line);
			std::vector<std::string> tokens = uskomaton::util::split(line, ' ');
			// connected
			if (tokens[1] == "004") {
				connected = true;
				break;
			}
		}

		if (connected) {
			read();
			inputThread = boost::thread(boost::bind(&boost::asio::io_service::run, &ioService));
		}
	}
	catch (boost::system::system_error& err) {

	}
}

void IrcClient::receive(boost::system::error_code const& e, size_t bytes) {
	std::istream is(&buffer);
	std::string line;
	std::getline(is, line);
	onRawMessage(line);
	read();
}

void IrcClient::read() {
	try {
		if (!socket.is_open()) return;
		boost::asio::async_read_until(socket, buffer, "\r\n", boost::bind(&IrcClient::receive, this, _1, _2));
	}
	catch (boost::system::system_error& e) {

	}
}

void IrcClient::connectTo(const std::string& hostname, int port) {
	connectTo(hostname, port, "");
}
void IrcClient::connectTo(const std::string& hostname) {
	connectTo(hostname, 6667, "");
}

void IrcClient::onRawMessage(const std::string& raw) {

	if (raw.empty()) return;

	std::string sourceraw;
	std::vector<std::string> tokens = uskomaton::util::split(raw, ' ');
	if (tokens.at(0).at(0) == ':') {
		sourceraw = tokens[0];
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
	if (target.size() != 0 && target.at(0) == ':') {
		target = target.substr(1);
	}
	// valid IRC line?
	if (sourceraw.size() != 0 && sourceraw.at(0) != ':') {
		// TODO unknown line
		return;
	}
	
	// TODO parse users etc
	processCommand(command, target, tokens);
	notifyListeners([&raw, &command, &target](IrcMessageListener* listener) {
		listener->onRawMessage(raw, command, target);
	});
}

void IrcClient::onServerPing(const std::string& ping) {
	sendRawMessage(std::string("PONG ") + ping);
	notifyListeners([&ping](IrcMessageListener* listener) {
		listener->onServerPing(ping);
	});
}

void IrcClient::sendMessage(const std::string& channel, const std::string& message) {
	std::string line = std::string("PRIVMSG ") + channel + std::string(" :") + message;
	sendRawMessage(line);
}

void IrcClient::sendRawMessage(const std::string& line) {
	std::string raw(line + "\r\n");
	std::cout << ">>" << raw;
	try {
		boost::asio::write(socket, boost::asio::buffer(raw.data(), raw.size()), boost::asio::transfer_all());
	}
	catch (boost::system::system_error& error) {

	}
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

void IrcClient::terminate() {
	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket.close();
	inputThread.join();
	outputThread.join();
}

const std::string& IrcClient::getServerName() const {
	return server;
}

void IrcClient::processCommand(const std::string& command, const std::string& target, std::vector<std::string>& tokens) {
	std::stringstream ss;
	std::string message;
	// remove target TODO may not be the best idea
	tokens.erase(tokens.begin());
	for (size_t i = 0; i < tokens.size(); i++) {
		if (i == tokens.size() - 1) {
			ss << tokens[i];
		}
		else {
			ss << tokens[i] << " ";
		}
	}
	if (ss.str().size() != 0) {
		message = ss.str();
		// remove : and newlines
		message = message.substr(1, message.size()  - 2);
	}
	// CTCP
	if (command == "PRIVMSG" && message.find('\x0001') == 0  && message.find('\x0001') == message.size() - 1) {
		std::cout << "CTCP VITTU" << std::endl;
	}
	// message to channel or to us
	else if (command == "PRIVMSG" && !target.empty()) {
		notifyListeners([&target, &message](IrcMessageListener* listener) {
			listener->onMessage(target, message, "sender_todo");
		});
	}
}
