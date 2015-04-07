#pragma once
#include <string>
#include "bot.hpp"
namespace uskomaton {
	/* pure C++ command, perl uses different */
	class Command {
	public:
		Command(const std::string& name);
		const std::string& getName() const;
		virtual void handleLine(Bot& bot, const std::string& channel, const std::string& sender) = 0;
	private:
		const std::string name;
	};

	class PerlCallback : public Command {
	public:
		PerlCallback(const std::string& name);
		void handleLine(Bot& bot, const std::string& channel, const std::string& sender);
	};
}