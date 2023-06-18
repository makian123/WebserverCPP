#pragma warning(disable:4146)

#include "server/server.h"
#include <iostream>
#include <classes/user/user.h>
#include <apis/apis.h>

int main() {
	Server srv("127.0.0.1", "8080");

	srv.RegisterFileCallback("index.html");
	srv.RegisterFileCallback("register.html");
	srv.RegisterFileCallback("login.html");
	srv.RegisterFileCallback("style.css");

	srv.RegisterAPI("user/register", RegisterUser);
	srv.RegisterAPI("user/login", LoginUser);
	srv.RegisterAPI("user/delete", DeleteUser);
	srv.RegisterAPI("user/rename", ChangeUsername);

	srv.RegisterAPI("guild/create", CreateGuild);
	srv.RegisterAPI("guild/delete", DeleteGuild);
	srv.RegisterAPI("guild/join", JoinGuild);
	srv.RegisterAPI("guild/leave", LeaveGuild);

	srv.RegisterAPI("post/create", CreatePost);
	srv.RegisterAPI("post/delete", DeletePost);
	srv.RegisterAPI("post/upvote", UpvotePost);
	srv.RegisterAPI("post/downvote", DownvotePost);

	srv.Start();

	std::string s;
	std::getline(std::cin, s);

	srv.Stop();
}