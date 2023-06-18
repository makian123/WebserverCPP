#pragma once

#include <server/network.h>
#include <server/server.h>

Packet RegisterUser(SOCKET sock, const Header &header, Server &srv);
Packet DeleteUser(SOCKET sock, const Header &header, Server &srv);
Packet LoginUser(SOCKET sock, const Header &header, Server &srv);
Packet ChangeUsername(SOCKET sock, const Header &header, Server &srv);