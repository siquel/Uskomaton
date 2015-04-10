#pragma once
#include <vector>
#include <string>
#include <sstream>
namespace uskomaton {
	namespace util {
		static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				if (!item.empty())
					elems.push_back(item);
			}
			return elems;
		}


		std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}

		static void userHostmaskFromRawLine(std::string* to, std::string& raw) {
			size_t exc = raw.find("!");
			size_t at = raw.find("@");
			if (exc != std::string::npos && at != std::string::npos) {
				to[0] = raw.substr(0, exc);
				to[1] = raw.substr(exc, at - exc);
				to[2] = raw.substr(at);
			}
			else {
				to[0] = raw;
				to[1] = "";
				to[2] = "";
			}
		}
	}
}