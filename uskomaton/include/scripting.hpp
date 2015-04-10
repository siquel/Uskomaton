#pragma once

namespace uskomaton {
	class Bot;
	namespace scripting {
		class ScriptingAPI {
		public:
			ScriptingAPI() {};
			virtual ~ScriptingAPI() {};
			virtual void initialize(uskomaton::Bot* bot) = 0;
			virtual void processRawMessage(const std::string& raw, const std::string& context, const std::string& command, const std::string& target) = 0;
			virtual void processOnMessage(const std::string& context, const std::string& channel, const std::string& message, const std::string& sender) = 0;
			virtual void autoloadFrom(const std::string& path) = 0;
		};
	}
}