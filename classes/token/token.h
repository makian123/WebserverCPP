#pragma once

#include <db/redis.h>
#include <string>

class Token {
private:
	std::string key;
	std::string value;
public:
	Token() = default;

	static Token Generate(RedisDB &context, const std::string &value = "1");
	static Token Load(RedisDB &context, const std::string &key);

	void Refresh(RedisDB &context);
	void Delete(RedisDB &context);

	std::string GetKey() const { return key; }
	std::string GetVal() const { return value; }

	Token &operator=(const Token &other) { this->key = other.GetKey(); this->value = other.GetVal(); return *this; }
	bool operator==(const Token &other) { return key == other.key && value == other.value; }
	bool operator!=(const Token &other) { return !(*this == other); }

	static const Token INVALID;
};