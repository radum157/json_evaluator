#pragma once

#include "parser/component.hpp"

/* Namespace containing classes and functions used for JSON expression evaluation */
namespace json_eval {

	/* Exception thrown when evaluating a JSON expression */
	class JSONExpressionError : public std::exception {
		std::string msg;

	public:
		explicit JSONExpressionError(const std::string &msg) : msg(msg) {}

		inline const char *what() const noexcept override
		{
			return msg.c_str();
		}
	};

	enum JSONOp {
		MIN, MAX, SIZE, ADD, SUB, MULT, DIV, SUBSCRIPT, GET, DEFAULT, INVALID
	};

	class JSONExpression {
		JSONOp opcode;

		std::string literal;
		std::vector<JSONExpression> subexpr;

	public:

		JSONExpression() { opcode = GET; }
		JSONExpression(const std::string &literal, JSONOp opcode)
			: opcode(opcode), literal(literal)
		{}
		JSONExpression(const JSONExpression &e)
		{
			opcode = e.opcode;
			literal = e.literal;
			subexpr = decltype(subexpr)(e.subexpr);
		}

		json::JSONComponent *evaluate(const std::vector<json::JSONObject *> &objects);

		std::string to_str()
		{
			std::string s;
			for (auto &&e : subexpr) {
				s += "->" + e.to_str() + "\n";
			}

			return std::to_string(opcode) + " " + literal + "\n" + s;
		}

		inline void add_subexpr(const JSONExpression &e)
		{
			subexpr.push_back(e);
		}

		inline void operator=(const JSONExpression &e)
		{
			opcode = e.opcode;
			literal = e.literal;
			subexpr = decltype(subexpr)(e.subexpr);
		}

	};

	class Evaluator {
		JSONExpression root;

		/* Splits the expression */
		void _evaluate(std::string &expr);

	public:
		explicit Evaluator(std::string &expr)
		{
			_evaluate(expr);
		}

		/**
		 * Executes the expression given an array of JSON objects
		 */
		inline json::JSONComponent *evaluate(const std::vector<json::JSONObject *> &objects)
		{
			try {
				return root.evaluate(objects);
			} catch (const JSONExpressionError &e) {
				throw;
			}
		}
	};


} // namespace json_eval

