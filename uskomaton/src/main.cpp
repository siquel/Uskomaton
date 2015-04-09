#include "bot.hpp"
#include "conf/config.hpp"
int main ( int argc, char **argv) {
	using namespace uskomaton;
	config::Configuration conf;
	config::Configuration::fromFile("server.conf", conf);
    Bot bot(conf);
	bot.initialize();
	bot.connectServers();
	std::string line;
	while (getline(std::cin, line)) {
		if (line == "q") {
			break;
		}
	}
	bot.terminate();
	return 0;
}