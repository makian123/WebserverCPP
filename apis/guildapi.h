#pragma once

#include <server/network.h>
#include <server/server.h>

Packet CreateGuild(SOCKET sock, const Header &header, Server &srv);
Packet DeleteGuild(SOCKET sock, const Header &header, Server &srv);
Packet JoinGuild(SOCKET sock, const Header &header, Server &srv);
Packet LeaveGuild(SOCKET sock, const Header &header, Server &srv);