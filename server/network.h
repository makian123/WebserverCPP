#pragma once

#include <string>
#include <WinSock2.h>
#include <map>

struct Header {
	enum class HeaderType: uint8_t {
		POST,
		GET
	};

	int responseCode;
	std::string responseMsg;

	HeaderType type;
	std::map<std::string, std::string> params;
	size_t contentLen;

	static Header BuildHeader(const std::string &str);
	std::string SerializeHeader();
};
struct Packet {
	Header header;
	std::string message;
	SOCKET dest;

	static Packet BuildPacket(SOCKET destination, const Header &header, const std::string &message = "");
	static Packet BadRequest(SOCKET destination);
	bool SendPacket();
};

struct Cookie {
	std::map<std::string, std::string> values;

	Cookie() = default;

	static Cookie Generate(std::string in);
	std::string &Serialize() const;
};