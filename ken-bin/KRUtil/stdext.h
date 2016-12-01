#pragma once

#include <string>

namespace std
{
	namespace krext
	{
		bool isNumber(const string &str);

		// throws
		// invalid_argument: empty string or include not number
		int toNumber(const string &str);

		string trim(const std::string &s);

		void toLowerCase(string &str);

		string& replaceAll(string &str, char pattern, char replace);

		string& replaceAll(string &str, const string &pattern, const string &replace);

	}
}

