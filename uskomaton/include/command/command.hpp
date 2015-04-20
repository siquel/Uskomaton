#pragma once
#include <string>

namespace uskomaton {
	class Bot;
	namespace command {
		class Command {
		public:
			Command(const std::string& name);
			virtual ~Command();
			const std::string& getName() const;
			virtual void handleLine(Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) = 0;
		private:
			const std::string name;
		};
	}
}