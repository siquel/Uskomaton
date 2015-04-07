#include "bot.hpp"

int main ( int argc, char **argv) {
	using namespace uskomaton;
    Bot bot("Uskomaton", "uskomaton");
	bot.initialize();
	bot.start();
	return 0;
}