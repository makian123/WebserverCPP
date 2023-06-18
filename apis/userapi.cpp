#include "userapi.h"
#include <server/server.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <classes/token/token.h>
#include <openssl/sha.h>

Packet RegisterUser(SOCKET sock, const Header &header, Server &srv) {
	std::stringstream ss;

	ss << "200 OK\r\n"
		<< "HTTP/1.1\r\n"
		<< "Connection: close\r\n"
		<< "\r\n";

	Packet packet = Packet::BuildPacket(
		sock,
		Header::BuildHeader(ss.str()),
		""
	);

	SQLDB &sql = srv.GetSQL();

	ss.str(std::string());

	try {
		std::string str(33, '\0');

		ss << "INSERT INTO USER(USERNAME, PASSWORD) VALUES ('" << header.params.at("Username") << "', '" 
			<< SHA256((unsigned char*)header.params.at("Password").c_str(), header.params.at("Password").size(), (unsigned char*)str.data()) << "');";
		if (!sql.Execute(ss.str())) return Packet::BadRequest(sock);
	}
	catch (std::exception &e) { return Packet::BadRequest(sock); }
	return packet;
}
Packet DeleteUser(SOCKET sock, const Header &header, Server &srv) {
	std::stringstream ss;

	ss << "200 OK\r\n"
		<< "HTTP/1.1\r\n"
		<< "Connection: close\r\n"
		<< "\r\n";

	Packet packet = Packet::BuildPacket(
		sock,
		Header::BuildHeader(ss.str()),
		""
	);
	ss.str(std::string());

	return packet;
}

static int LoginCallback(void *value, int nCols, char **colnames, char **vals) {
	*(static_cast<bool *>(value)) = nCols > 0;
	return 0;
}
Packet LoginUser(SOCKET sock, const Header &header, Server &srv){
	std::stringstream ss;

	ss << "200 OK\r\n"
		<< "HTTP/1.1\r\n"
		<< "Connection: close\r\n"
		<< "\r\n";

	Packet packet;
	SQLDB &sql = srv.GetSQL();

	try {

		std::string pwHashed(33, '\0');
		SHA256((unsigned char *)header.params.at("Password").c_str(), header.params.at("Password").size(), (unsigned char *)pwHashed.data());

		std::stringstream newSS;
		newSS << "SELECT * FROM USER WHERE USERNAME = '" << header.params.at("Username") << "' AND PASSWORD = '" << pwHashed << "';";

		bool userFound = false;
		sql.Execute(newSS.str(), LoginCallback, &userFound);

		if(!userFound) return Packet::BadRequest(sock);
	}
	catch (std::exception &e) {
		return Packet::BadRequest(sock);
	}
	Header generatedHeader = Header::BuildHeader(ss.str());
	generatedHeader.params["Set-Cookie"] = std::string("token=") + Token::Generate(srv.GetRedis()).GetKey();

	packet = Packet::BuildPacket(sock,generatedHeader);
	return packet;
}
Packet ChangeUsername(SOCKET sock, const Header &header, Server &srv) {
	std::stringstream ss;

	ss << "200 OK\r\n"
		<< "HTTP/1.1\r\n"
		<< "Connection: close\r\n"
		<< "\r\n";

	Packet packet = Packet::BuildPacket(
		sock,
		Header::BuildHeader(ss.str()),
		""
	);

	Cookie cookie;

	try {
		cookie = Cookie::Generate(header.params.at("Cookie"));

		Token tok = Token::Load(srv.GetRedis(), cookie.values.at("token"));
		if (tok == Token::INVALID || tok.GetKey() != cookie.values["token"] || tok.GetVal() != header.params.at("Username")) {
			packet.header.responseCode = 400;
			packet.header.responseMsg = "Bad request";
			return packet;
		}


	}
	catch (std::exception &e) {
		return Packet::BadRequest(sock);
	}

	return packet;
}