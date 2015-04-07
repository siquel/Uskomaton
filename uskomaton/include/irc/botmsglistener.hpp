#pragma once
#include "ircclient.hpp"
namespace uskomaton {
	class Bot;
}
class BotMessageListener : public IrcMessageListener {
private:
	uskomaton::Bot* bot;
public:
	BotMessageListener(uskomaton::Bot* bot);
	void onRawMessage(const std::string& msg, const std::string& command, const std::string& target) override;
};