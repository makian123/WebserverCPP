#include "sql.h"

#include <iostream>

bool SQLDB::Execute(const std::string &code, void *callback, void *data) {
	if (!db) {
		std::cout << "Database not opened\n";
		return false;
	}

	char *errmsg;
	int errCode = sqlite3_exec(db, code.c_str(), static_cast<int(*)(void*,int,char**,char**)>(callback), data, &errmsg);
	if (errCode != SQLITE_OK) {
		std::cout << "Error executing '" << code << "': " << sqlite3_errstr(errCode) << "\n";
		return false;
	}

	return true;
}

void SQLDB::Open(const std::string &path) {
	int errCode = sqlite3_open(path.c_str(), &db);

	if (!db) {
		std::cout << "Error opening '" << path << "': " << sqlite3_errstr(errCode) << "\n";
		return;
	}

	char *errmsg;
	//Enable foreign keys
	sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, 1, &errmsg);
}
void SQLDB::Close() {
	if(db) sqlite3_close(db);
	db = nullptr;
}