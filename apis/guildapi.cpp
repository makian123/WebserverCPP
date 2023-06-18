#include "guildapi.h"
#include <sstream>
#include <iostream>
#include <classes/token/token.h>

Packet CreateGuild(SOCKET sock, const Header &header, Server &srv){
	std::stringstream ss;

	try { header.params.at("Cookie"); }
	catch (std::exception &e) { std::cout << e.what(); return Packet::BadRequest(sock); }

	Cookie cookie = Cookie::Generate(header.params.at("Cookie"));

	try {
		Token tok;
		tok = Token::Load(srv.GetRedis(), cookie.values.at("token"));

		ss << "INSERT INTO GUILD(OWNERID, NAME) VALUES('" << tok.GetVal() << "', '" << header.params.at("GuildName") << "');";
	}
	catch (...) { return Packet::BadRequest(sock); }

	SQLDB &db = srv.GetSQL();
	if (!db.Execute(ss.str())) return Packet::BadRequest(sock);

	ss.str() = std::string();

	ss  << "200 OK\r\n"
		<< "HTTP/1.1\r\n"
		<< "Connection: close\r\n"
		<< "\r\n";

	return Packet::BuildPacket(sock, Header::BuildHeader(ss.str()));
}
Packet DeleteGuild(SOCKET sock, const Header &header, Server &srv){
	return Packet::BadRequest(sock);
}
Packet JoinGuild(SOCKET sock, const Header &header, Server &srv){
	return Packet::BadRequest(sock);
}
Packet LeaveGuild(SOCKET sock, const Header &header, Server &srv){
	return Packet::BadRequest(sock);
}