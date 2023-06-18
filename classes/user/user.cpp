#include "user.h"
#include <sstream>
#include <vector>

#include <iostream>

using Col = std::string;
using Record = std::vector<Col>;
using Records = std::vector<Record>;

static int UserCallback(void *p_data, int num_fields, char **p_fields, char **p_col_names) {
	Record *rec = reinterpret_cast<Record *>(p_data);

	for (int i = 0; i < num_fields; ++i) {
		rec->emplace_back(std::string(p_fields[i]));
	}

	return 0;
}

User::User(SQLDB &context, int64_t id) {
	std::stringstream ss;
	ss << "SELECT * FROM USER WHERE ROWID = " << id << ";";
	Record record;

	context.Execute(ss.str(), UserCallback, &record);
	if(record.size()) {
		this->id = id;
		this->name = record.at(1);
		this->pwHash = std::stoll(record.at(2).c_str());
	}
	else {
		std::cout << "User not found\n";
	}
}
User::User(SQLDB &context, const std::string &username, int64_t pwHash) {
	std::stringstream ss;
	ss << "SELECT ROWID FROM USER WHERE USERNAME = '" << username << "' AND PASSWORD = " << pwHash << ";";
	Record record;

	context.Execute(ss.str(), UserCallback, &record);
	if (record.size()) {
		*this = User(context, std::stoll(record.at(0).c_str()));
	}
}

User User::CreateUser(SQLDB &context, const std::string &username, int64_t pwHash) {
	std::stringstream ss;
	ss << "INSERT INTO USER(USERNAME, PASSWORD) VALUES('" << username << "', " << pwHash << ");";

	if (!context.Execute(ss.str())) {
		return User();
	}
	
	return User(context, context.GetLastRow());
}
void User::DeleteUser(SQLDB &context, int id) {
	std::stringstream ss;
	ss << "DELETE FROM USER WHERE ROWID = " << id << ";";

	context.Execute(ss.str());
}

void User::Save(SQLDB &context) {
	std::stringstream ss;
	ss << "UPDATE TABLE USER SET USERNAME = '" << name << "', PASSWORD = '" << pwHash << "' WHERE ROWID = " << id << ";";

	context.Execute(ss.str());
}
void User::Delete(SQLDB &context) {
	if (id < 1) return;

	std::stringstream ss;
	ss << "DELETE FROM USER WHERE ROWID = " << id << ";";

	context.Execute(ss.str());
}