#pragma once
#include "ircclient.hpp"
#include "conf/config.hpp"
#include "command/command_interface.hpp"
#include "forward_declare.hpp"
USKOMATON_FORWARD_DECL_1(uskomaton, Bot);

class BotMessageListener : public IrcMessageListener {
private:
	uskomaton::Bot* bot;
	uskomaton::config::ServerConfiguration config;
	uskomaton::command::CommandInterface commandInterface;
public:
	BotMessageListener(uskomaton::Bot* bot, const uskomaton::config::ServerConfiguration&);
	void onRawMessage(const std::string& msg, const std::string& command, const std::string& target) override;
	void onMessage(const std::string& channel, const std::string& message, const std::string& sender) override;
	void onPrivateMessage(const std::string& target, const std::string& message, const std::string& sender) override;
	void onJoinChannel(const std::string& channel, const std::string& sender);
};