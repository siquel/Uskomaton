#pragma once
#include <string>
#include <vector>
namespace uskomaton {
	namespace scripting {
		class Script {
		public:
			Script(const std::string& filename, const std::string& name);
			~Script();
			const std::string& getFilename() const;
			const std::string& getName() const;
		private:
			const std::string filename;
			const std::string name;
		};
	}
}