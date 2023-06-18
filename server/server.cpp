#include "server.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <functional>
#include <apis/userapi.h>
#include <classes/token/token.h>

const std::string Server::redisIP = "127.0.0.1";
const uint16_t Server::redisPort = 6379;

void Server::Listen() {
	std::unique_ptr<SOCKET> tmpSock;
	sockaddr tmpAddr;

	while (isRunning.load()) {
		if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) continue;
		SOCKET resSock = accept(listenSock, nullptr, nullptr);
		if (resSock != INVALID_SOCKET) {
			tmpSock = std::make_unique<SOCKET>(resSock);

			connections.emplace_back(
				&Server::HandleConnection,
				this, std::move(tmpSock), tmpAddr
			);
			tmpSock = nullptr;
		}

		for (size_t i = 0; i < connections.size(); ++i) {
			auto &thread = connections[i];

			if (thread.joinable()) {
				thread.join();
				connections.erase(connections.begin() + i);
				--i;
			}
		}
	}
}
void Server::HandleConnection(std::unique_ptr<SOCKET> sock, sockaddr addr) {
	std::string buffer;
	buffer.resize(4096);

	while (!recv(*sock.get(), &buffer[0], 4096, MSG_PEEK));
	
	std::stringstream wsss;

	Header header = Header::BuildHeader(buffer);
	if (header.params.find("path") != header.params.end()) {
		std::string &path = header.params["path"];
		path.erase(0, 1);
		path = path.substr(0, path.find('?'));

		if (callbacks.find(path) != callbacks.end()) {
			callbacks[path](*sock.get(), header, *this).SendPacket();

			shutdown(*sock.get(), SD_SEND);
			return;
		}
	}

	std::cout << header.params["path"] << " not found\n";

	wsss << "404 Page not found\r\n"
		<< "HTTP/1.1\r\n"
		<< "Connection: close\r\n"
		<< "Content-Type: text/plain\r\n"
		<< "\r\n";
	header = Header::BuildHeader(wsss.str());

	Packet::BuildPacket(*sock.get(), Header::BuildHeader(wsss.str()), "").SendPacket();

	shutdown(*sock.get(), SD_SEND);
}

User Server::CreateUser(const std::string &token, const std::string &username, int64_t password) {
	static std::atomic_bool occupied { false };

	while (occupied.load());
	occupied.store(true);

	User usr = User::CreateUser(sql, username, password);

	occupied.store(false);

	return usr;
}
bool Server::ValidateLogin(const std::string &username, int64_t password) {
	static std::atomic_bool occupied{ false };

	while (occupied.load());
	occupied.store(true);

	bool retVal = (User::User(sql, username, password).GetID() > 0);

	occupied.store(false);

	return retVal;
}
void Server::RegisterFileCallback(const std::string &path) {
	std::string fileType = path.substr(path.find('.') + 1);
	if (!path.size() || !fileType.size()) return;

	if (fileType == "html") fileType = "text/html";
	else if (fileType == "js") fileType = "text/javascript";
	else if (fileType == "css") fileType = "text/css";
	else fileType = "text/mime";

	callbacks[path] = std::function<Packet (SOCKET, const Header&, Server &)>(
		[path, fileType](SOCKET sock, const Header &h, Server &srv) {
			std::stringstream ss;

			ss << "200 OK\r\n"
				<< "HTTP/1.1\r\n"
				<< "Connection: close\r\n"
				<< "Content-Type: " << fileType << "\r\n"
				<< "\r\n";
			Header header = Header::BuildHeader(ss.str());

			std::ifstream file("website/" + path);
			Packet packet = Packet::BuildPacket(
				sock,
				Header::BuildHeader(ss.str()),
				std::string(
					(std::istreambuf_iterator<char>(file)),
					(std::istreambuf_iterator<char>())));
			file.close();

			return packet;
		});
}

void Server::Start() {
	if (isRunning.load()) return;
	isRunning.store(true);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	addrinfo *res, *ptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);

	listenSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(listenSock, res->ai_addr, (int)res->ai_addrlen);
	freeaddrinfo(res);

	redis.Open(redisIP, redisPort);

	sql.Open("DB.db");

	bool result = true;
	result = sql.Execute(
		"CREATE TABLE IF NOT EXISTS USER("
		"ROWID INTEGER NOT NULL,"
		"USERNAME TEXT NOT NULL,"
		"PASSWORD CHAR(33) NOT NULL,"
		"PRIMARY KEY(ROWID)"
		");"
	);

	result = sql.Execute(
		"CREATE TABLE IF NOT EXISTS POST("
		"ROWID INTEGER NOT NULL,"
		"MESSAGE TEXT NOT NULL,"
		"AUTHORID BIGINT NOT NULL,"
		"GUILD BIGINT NOT NULL,"
		"PRIMARY KEY(ROWID),"
		"FOREIGN KEY (AUTHORID) REFERENCES USER(ROWID),"
		"FOREIGN KEY (GUILD) REFERENCES GUILD(ROWID)"
		");"
	);

	result = sql.Execute(
		"CREATE TABLE IF NOT EXISTS GUILD("
		"ROWID INTEGER NOT NULL,"
		"OWNERID INTEGER NOT NULL,"
		"NAME TEXT NOT NULL,"
		"PRIMARY KEY(ROWID),"
		"FOREIGN KEY (OWNERID) REFERENCES USER(ROWID)"
		");"
	);

	result = sql.Execute(
		"CREATE TABLE IF NOT EXISTS GUILDS_USERS("
		"USERID INTEGER NOT NULL,"
		"GUILDID INTEGER NOT NULL,"
		"PRIMARY KEY(USERID, GUILDID)"
		");"
	);

	result = sql.Execute(
		"CREATE TABLE IF NOT EXISTS USER_POSTS("
		"USERID INTEGER NOT NULL,"
		"POSTID INTEGER NOT NULL,"
		"PRIMARY KEY(USERID, POSTID)"
		");"
	);

	listener = std::thread(&Server::Listen, this);
}
void Server::Stop(){
	if (!isRunning.load()) return;
	isRunning.store(false);

	listener.join();
	for (auto &conn : connections) {
		conn.join();
	}

	sql.Close();
	redis.Close();

	WSACleanup();
}

Server::Server(const std::string &ip, const std::string &port)
	:ip(ip), port(port) {
	isRunning.store(false);
}