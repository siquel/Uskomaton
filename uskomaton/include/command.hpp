#pragma once
#include <string>
namespace uskomaton {
	/* pure C++ command, perl uses different */
	class Command {
	public:
		const std::string& getName() const;
	private:
		const std::string name;
	};
}