#include "command.hpp"

namespace uskomaton {
	class Bot;
	namespace command {
		class UnloadCommand : public Command {
		public:
			UnloadCommand();
			void handleLine(Bot& bot, const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) override;
		};
	}
}