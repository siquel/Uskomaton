#pragma once

namespace uskomaton {
	class Bot;
	namespace scripting {
		class ScriptingAPI {
		public:
			ScriptingAPI() {};
			virtual ~ScriptingAPI() {};
			virtual void initialize(uskomaton::Bot* bot) = 0;
			virtual void processRawMessage(const std::string& raw) = 0;
		};
	}
}