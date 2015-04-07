#include "bot.hpp"

int main ( int argc, char **argv) {
	using namespace uskomaton;
    Bot bot("Uskomaton", "uskomaton");
	bot.initialize();
	bot.connectTo("irc.example.org");
	bot.joinChannel("#dev");
	std::string line;
	while (getline(std::cin, line)) {
		if (line == "q") {
			break;
		}
	}
	bot.terminate();
	return 0;
}