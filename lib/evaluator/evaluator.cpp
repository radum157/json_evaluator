#include <algorithm>
#include <iostream>

#include "evaluator/evaluator.hpp"

using std::string;

namespace json_eval {

	/** Remove all whitespaces from @a str */
	inline void _strip(string &str)
	{
		str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
	}

	/**
	 * @return If the string contains the correct amount of opening / closing parentheses
	 */
	bool _valid_paranthesis(const string &str)
	{
		int open_sq = 0;
		int open = 0;

		for (char c : str) {
			switch (c) {
			case '(': open++; break;
			case ')':
				open--;
				if (open < 0) {
					return false;
				}
				break;
			case '[': open_sq++; break;
			case ']':
				open_sq--;
				if (open_sq < 0) {
					return false;
				}
				break;
			default:
				break;
			}
		}

		return true;
	}

	/**
	 * Gets the opcode for a given keyword
	 */
	JSONOp _get_opcode(const string &str)
	{
		if (str == "min" || str == "MIN") {
			return MIN;
		}
		if (str == "max" || str == "MAX") {
			return MAX;
		}
		if (str == "size" || str == "SIZE") {
			return SIZE;
		}
		return INVALID;
	}

	size_t _find_close(const string &str, size_t start, char p)
	{
		int cnt = 1;
		for (size_t i = start; i < str.size(); i++) {
			if (str[i] == p) {
				cnt++;
			}

			if (p == '(' && str[i] == ')') {
				cnt--;
			} else if (p == '[' && str[i] == ']') {
				cnt--;
			}

			if (cnt == 0) {
				return i;
			}
		}

		return string::npos;
	}

	/**
	 * Actual expression splitting. Recursive
	 */
	void _split_expr(string &expr, JSONExpression &root, size_t start = 0)
	{
		static const string op = "+-*/([.";

		auto pos = expr.find_first_of(op, start);
		if (pos == string::npos) {
			root = JSONExpression(expr.substr(start), GET);
		}

		switch (expr[pos]) {
			// TODO: implement these
		case '[':
			break;
		case '(':
			break;
		case '.':
			break;

			// TODO: arithmetic

		default:
			root = JSONExpression(expr.substr(start), GET);
			return;
		}
	}

	void Evaluator::_evaluate(string &expr)
	{
		if (!_valid_paranthesis(expr)) {
			throw JSONExpressionError("invalid paranthesis");
		}

		// whitespaces not supported
		_strip(expr);

		_split_expr(expr, root);

		std::cout << root.to_str() << '\n';
	}

	json::JSONObject *_find(const std::vector<json::JSONObject *> &objects, const string &name)
	{
		for (auto &&obj : objects) {
			if (obj->get_name() == name) {
				return obj;
			}
		}

		return nullptr;
	}

	json::JSONComponent *JSONExpression::evaluate(const std::vector<json::JSONObject *> &objects)
	{
		if (opcode == INVALID || literal.empty()) {
			throw JSONExpressionError("invalid opcode or literal");
		}

		json::JSONObject *obj;

		switch (opcode) {
		case DEFAULT:
			return _find(objects, literal);

		case GET:
			if (subexpr.size() != 1) {
				throw JSONExpressionError("invalid access expression");
			}

			obj = _find(objects, literal);
			if (!obj || !dynamic_cast<const json::JSONObject *>(obj->get_value())) {
				throw JSONExpressionError("invalid object field");
			}

			return subexpr[0].evaluate({ (json::JSONObject *)obj->get_value() });

			// TODO: rest of the cases
		default:
			return nullptr;
		}
	}

} // namespace json_eval

