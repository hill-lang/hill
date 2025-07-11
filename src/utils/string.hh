#ifndef HILL__UTILS__STRING_HH_INCLUDED
#define HILL__UTILS__STRING_HH_INCLUDED

#include <algorithm>
#include <cctype>
#include <ios>
#include <locale>
#include <string>
#include <sstream>

namespace hill::utils {

	inline std::string escape_string(const char *s)
	{
		std::stringstream ss;
		while (*s) {
			switch (*s) {
			case '"': ss << "\\\""; break;
			case '\n': ss << "\\n"; break;
			default: ss << *s;
			}
			++s;
		}
		return ss.str();
	}

	inline std::string xml_escape_string(const char *s)
	{
		std::stringstream ss;
		while (*s) {
			switch (*s) {
			case '"': ss << "&quot;"; break;
			case '\'': ss << "&apos;"; break;
			case '<': ss << "&lt;"; break;
			case '>': ss << "&gt;"; break;
			case '&': ss << "&amp"; break;
			default: ss << *s;
			}
			++s;
		}
		return ss.str();
	}

	/// <summary>
	/// Trim leading whitespace (in place)
	/// </summary>
	inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
	}

	/// <summary>
	/// Trim trailing whitespace (in place)
	/// </summary>
	inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	inline void trim(std::string &s)
	{
		ltrim(s);
		rtrim(s);
	}
}

#endif /* HILL__UTILS__STRING_HH_INCLUDED */
