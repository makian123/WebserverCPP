#pragma once

#include <db/sql.h>
#include <string>

class User{
private:
	int64_t id = -1;
	std::string name = "";
	int64_t pwHash = -1;
public:
	explicit User() = default;
	explicit User(SQLDB &context, int64_t id);
	User(SQLDB &context, const std::string &username, int64_t pwHash);

	static User CreateUser(SQLDB &context, const std::string &username, int64_t pwHash);
	static void DeleteUser(SQLDB &context, int id);

	void SetName(const std::string &name) { this->name = name; }
	void SetPw(int64_t hash) { this->pwHash = hash; }

	int GetID() const { return id; }
	std::string GetName() const { return name; }
	int64_t GetPw() const { return pwHash; }


	void Save(SQLDB &context);
	void Delete(SQLDB &context);
};