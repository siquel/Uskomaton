#include "bot.hpp"

int main ( int argc, char **argv, char **env )
{
	using namespace uskomaton;
    Bot bot("Uskomaton", "uskomaton");
	bot.initialize();
	return 0;
}