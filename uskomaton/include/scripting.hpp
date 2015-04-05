#pragma once

namespace uskomaton {
	namespace scripting {
		class ScriptingAPI {
		public:
			ScriptingAPI() {};
			virtual ~ScriptingAPI() {};
			virtual void initialize() = 0;
		};
	}
}