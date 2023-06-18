#pragma once

#include <server/network.h>
#include <server/server.h>

Packet CreatePost(SOCKET sock, const Header &header, Server &srv);
Packet DeletePost(SOCKET sock, const Header &header, Server &srv);
Packet UpvotePost(SOCKET sock, const Header &header, Server &srv);
Packet DownvotePost(SOCKET sock, const Header &header, Server &srv);