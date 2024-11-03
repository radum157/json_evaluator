#include <vector>
#include <fstream>
#include <unordered_set>

#include "parser/parser.hpp"

namespace json {

	JSONComponent *_read_next(std::ifstream &, bool);

	/**
	 * @return First available character in stream or -1 if eof
	 */
	char _skip_ws(std::ifstream &fin)
	{
		char c;
		while (fin >> c) {
			if (!std::isspace(c)) {
				return c;
			}
		}

		return -1;
	}

	/**
	 * @return Escaped version of @a c
	 */
	char _escape_ch(char c)
	{
		switch (c) {
		case 'n': return '\n';
		case 't': return '\t';
		case 'r': return '\r';
		case 'b': return '\b';
		case 'f': return '\f';
		case 'a': return '\a';
		case 'v': return '\v';
		case '\\': return '\\';
		case '\'': return '\'';
		case '\"': return '\"';
		case '0': return '\0';
		default: return c;
		}
	}

	/**
	 * @return String read from current position in @a fin (until '"' is reached)
	 * @throws JSONParseError if string is invalid
	 */
	std::string _parse_string(std::ifstream &fin)
	{
		std::string str;

		char c;
		while (fin >> c) {
			if (c < 0 || !std::isprint(c)) {
				throw JSONParseError("invalid file data");
			}

			if (!str.empty() && str.back() == '\\') {
				str[str.size() - 1] = _escape_ch(c);
				continue;
			}
			if (c == '"') {
				break;
			}

			str += c;
		}

		return str;
	}

	/**
	 * @return JSONObject read from current position in @a fin. Reads recursively.
	 * @throws JSONParseError if object is invalid
	 */
	JSONObject *_read_object(std::ifstream &fin)
	{
		// Name
		char c = _skip_ws(fin);
		if (c != '"') {
			throw JSONParseError("invalid format");
		}

		auto obj = new JSONObject();
		try {
			obj->set_name(_parse_string(fin));
		} catch (const JSONParseError &e) {
			throw;
		}

		c = _skip_ws(fin);
		if (c != ':') {
			throw JSONParseError("invalid format");
		}

		// Value
		try {
			auto value = _read_next(fin, true);
			obj->set_value(value);
		} catch (const JSONParseError &e) {
			throw;
		}

		return obj;
	}

	/**
	 * @return JSONArray read from current position in @a fin. Reads recursively.
	 * @throws JSONParseError if object is invalid
	 */
	JSONArray *_read_array(std::ifstream &fin)
	{
		auto array = new JSONArray();

		try {
			JSONComponent *comp = _read_next(fin, true);
			while (comp) {
				array->add_value(comp);

				char c = _skip_ws(fin);
				if (c != ',' && c != ']') {
					throw JSONParseError("invalid format");
				}

				if (c == ']') {
					break;
				}

				comp = _read_next(fin, true);
			}
		} catch (const JSONParseError &e) {
			throw;
		}

		return array;
	}

	/**
	 * @return JSONString read from current position in @a fin. Wrapper over _parse_string
	 * @throws JSONParseError if object is invalid
	 */
	JSONString *_read_string(std::ifstream &fin)
	{
		try {
			std::string str = _parse_string(fin);
			return new JSONString("\"" + str + "\"");
		} catch (const JSONParseError &e) {
			throw;
		}
	}

	/**
	 * @return JSONNumber read from current position in @a fin.
	 * @throws JSONParseError if object is invalid
	 */
	JSONNumber *_read_number(std::ifstream &fin)
	{
		fin.seekg(-1, std::ios::cur);

		long x;
		if (!(fin >> x)) {
			throw JSONParseError("invalid file data");
		}
		return new JSONNumber(x);
	}

	/**
	 * Attempts to read the next JSONComponent in the file. This is used for further recursions when reading objects / arrays
	 * @throws JSONParseError if file is not formatted correctly
	 * @param nested if the call is inside a recursion. Used to check what component is expected next
	 */
	JSONComponent *_read_next(std::ifstream &fin, bool nested = false)
	{
		char c = _skip_ws(fin);
		if (c == -1 || c == ']') {
			return nullptr;
		}
		if (!nested && c != '"') {
			// Need object
			throw JSONParseError("invalid file data");
		}

		JSONObject *obj;

		try {
			switch (c) {
			case '{':
				obj = _read_object(fin);

				// cleanup on stream
				c = _skip_ws(fin);
				if (c != '}') {
					throw JSONParseError("invalid format");
				}

				return obj;

			case '[':
				return _read_array(fin);
			case '"':
				return _read_string(fin);
			default:
				return _read_number(fin);
			}
		} catch (const JSONParseError &e) {
			throw;
		}
	}

	std::vector<JSONObject *> parse_file(const char *filename)
	{
		std::ifstream fin(filename);
		if (!fin.is_open()) {
			throw JSONParseError("could not open file");
		}

		char c = _skip_ws(fin);
		if (c != '{') {
			throw JSONParseError("invalid format");
		}

		std::vector<JSONObject *> res;

		// start reading all objects
		try {
			JSONObject *comp = _read_object(fin);
			while (comp) {
				res.push_back(comp);

				c = _skip_ws(fin);
				if (c == '}') {
					break;
				}

				if (c != ',') {
					throw JSONParseError("invalid format");
				}

				comp = _read_object(fin);
			}
		} catch (const JSONParseError &e) {
			throw;
		}

		fin.close();
		return res;
	}

} // namespace json
