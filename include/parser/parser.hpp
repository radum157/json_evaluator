#pragma once

#include <vector>

#include "component.hpp"

namespace json {

	/**
	 * Exception thrown by all (or most) parsing functions in namespace json
	 */
	class JSONParseError : public std::exception {
		std::string msg;

	public:
		explicit JSONParseError(const std::string &msg) : msg(msg) {}

		inline const char *what() const noexcept override
		{
			return msg.c_str();
		}
	};

	/**
	 * Attempts to open and parse a JSON-formatted file.
	 * @return An array of JSONObjects, matching the content of the file
	 * @throws JSONParseError if file is not formatted correctly
	 */
	std::vector<JSONObject *> parse_file(const char *filename);

} // namespace json
