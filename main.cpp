#include <iostream>
#include <fstream>
#include <thread>
#include <future>

#include "parser/parser.hpp"
#include "evaluator/evaluator.hpp"

void cleanup(const std::vector<json::JSONObject *> &objects)
{
	for (auto &&obj : objects) {
		delete obj;
	}
}

int main(int argc, char const *argv[])
{
	if (argc != 3) {
		std::cerr << "Usage: ./json_eval <json_file> <expression>\n";
		return 1;
	}

	// start parser
	auto parse_res = std::async(std::launch::async, json::parse_file, argv[1]);

	// start evaluator
	std::string expr(argv[2]);
	auto ev = json_eval::Evaluator(expr);

	std::vector<json::JSONObject *> objects;

	// get parser result
	try {
		objects = parse_res.get();
	} catch (const json::JSONParseError &e) {
		std::cerr << "JSONParseError: " << e.what() << '\n';
		return 1;
	}

	try {
		auto res = ev.evaluate(objects);
		if (!res) {
			// should not get here
			cleanup(objects);
			return 1;
		}

		std::cout << res->to_str() << '\n';
		delete res;
	} catch (const json_eval::JSONExpressionError &e) {
		std::cerr << "JSONExpressionError: " << e.what() << '\n';
		cleanup(objects);
		return 1;
	}

	cleanup(objects);
	return 0;
}
