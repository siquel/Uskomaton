#pragma once
#include "ircclient.hpp"
#include "conf/config.hpp"
namespace uskomaton {
	class Bot;
}
class BotMessageListener : public IrcMessageListener {
private:
	uskomaton::Bot* bot;
	uskomaton::config::ServerConfiguration config;
public:
	BotMessageListener(uskomaton::Bot* bot, const uskomaton::config::ServerConfiguration&);
	void onRawMessage(const std::string& msg, const std::string& command, const std::string& target) override;
};