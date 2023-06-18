#pragma once

#include <deps/hiredis/hiredis.h>
#include <string>

class RedisDB {
private:
	redisContext *db = nullptr;

public:
	RedisDB() = default;

	std::string Execute(const std::string &code);

	void Open(const std::string &ip, uint16_t port);
	void Close();
};