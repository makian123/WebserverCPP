#include "redis.h"

#include <iostream>

std::string RedisDB::Execute(const std::string &code) {
	if (!db) {
		std::cout << "Database not opened\n";
		return "";
	}

	redisReply *reply = static_cast<redisReply*>(redisCommand(db, code.c_str()));
	if (!reply) {
		std::cout << "Error executing '" << code << "'\n";
		return "";
	}

	std::string ret = reply->str;

	freeReplyObject(reply);

	return ret;
}

void RedisDB::Open(const std::string &ip, uint16_t port) {
	db = redisConnect(ip.c_str(), port);
	if (!db) {
		std::cout << "Error opening redis\n";
		return;
	}
}

void RedisDB::Close(){
	if (db) redisFree(db);
	db = nullptr;
}