#pragma once

#include <string>
#include <vector>

/* Namespace containing functions useful for parsing JSON files */
namespace json {

	/**
	 * Generic JSON component. All inheritors are possible objects found in a JSON file
	 */
	class JSONComponent {
	public:
		virtual ~JSONComponent() = default;
		/* For visualisation etc. */
		virtual std::string to_str() const noexcept = 0;
	};

	class JSONString : public JSONComponent {
		std::string value;

	public:
		explicit JSONString(const std::string &value) : value(value) {}

		inline std::string to_str() const noexcept override
		{
			return value;
		}
	};

	class JSONNumber : public JSONComponent {
		__INT64_TYPE__ value;

	public:
		explicit JSONNumber(const __INT64_TYPE__ &value) : value(value) {}

		inline std::string to_str() const noexcept override
		{
			return std::to_string(value);
		}
	};

	class JSONArray : public JSONComponent {
		std::vector<JSONComponent *> values;

	public:
		explicit JSONArray() {}
		explicit JSONArray(const std::vector<JSONComponent *> &values) : values(values) {}
		~JSONArray()
		{
			for (auto &&val : values) {
				delete val;
			}
		}

		std::string to_str() const noexcept override
		{
			std::string str = "[ ";
			for (auto &&val : values) {
				str += val->to_str() + ", ";
			}

			str.resize(str.size() - 2);
			return str + " ]";
		}

		void add_value(JSONComponent *comp)
		{
			values.push_back(comp);
		}
	};

	class JSONObject : public JSONComponent {
		std::string name;
		JSONComponent *value;

	public:
		JSONObject() : name(""), value(nullptr) {}
		JSONObject(const std::string &name, JSONComponent *value) : name(name), value(value) {}
		~JSONObject()
		{
			delete value;
		}

		inline std::string to_str() const noexcept override
		{
			return "{\"" + name + "\": " + value->to_str() + "}";
		}

		inline void set_name(const std::string &name)
		{
			this->name = name;
		}

		inline void set_value(JSONComponent *value)
		{
			this->value = value;
		}

		inline std::string get_name() const
		{
			return name;
		}

		inline const JSONComponent *get_value() const
		{
			return value;
		}
	};

} // namespace json
