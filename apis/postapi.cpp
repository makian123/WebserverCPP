#include "postapi.h"

Packet CreatePost(SOCKET sock, const Header &header, Server &srv){
	return Packet::BadRequest(sock);
}
Packet DeletePost(SOCKET sock, const Header &header, Server &srv){
	return Packet::BadRequest(sock);
}
Packet UpvotePost(SOCKET sock, const Header &header, Server &srv){
	return Packet::BadRequest(sock);
}
Packet DownvotePost(SOCKET sock, const Header &header, Server &srv){
	return Packet::BadRequest(sock);
}