#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include <map>
#include <functional>
#include <WinSock2.h>

#include <db/redis.h>
#include <db/sql.h>
#include <classes/user/user.h>
#include "network.h"

class Server{
private:
	static const std::string redisIP;
	static const uint16_t redisPort;


	std::string ip, port;
	std::thread listener;

	SQLDB sql;
	RedisDB redis;
	SOCKET listenSock;

	std::map<std::string, std::function<Packet (SOCKET, const Header&, Server&)>> callbacks;
	std::vector<std::thread> connections;
	std::atomic_bool isRunning;

	void Listen();
	void HandleConnection(std::unique_ptr<SOCKET> sock, sockaddr addr);
public:
	Server(const std::string &ip, const std::string &port);

	User CreateUser(const std::string &token, const std::string &username, int64_t password);
	bool ValidateLogin(const std::string &username, int64_t password);
	void RegisterFileCallback(const std::string &path);
	void RegisterAPI(const std::string &path, std::function<Packet(SOCKET, const Header&, Server&)> func) { callbacks[path] = func; }

	void Start();
	void Stop();

	SQLDB &GetSQL() { return sql; }
	RedisDB &GetRedis() { return redis; }

	size_t GetConnection() const { return connections.size(); }
};