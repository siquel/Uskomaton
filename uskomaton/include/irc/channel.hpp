#pragma once
#include <string>
#include <vector>
namespace uskomaton {
	namespace irc {
		class Channel {
		public:
			Channel();
			// channels topic
			const std::string& getTopic() const;
			// channels name
			const std::string& getName() const;
			// returns all the nicks who are on this channel
			const void getNicks(std::vector<std::string>& nicks) const;
		private:
			const std::string name;
			std::string topic;
		};
	}
}