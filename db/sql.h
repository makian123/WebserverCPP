#pragma once

#include <deps/sqlite3/sqlite3.h>
#include <string>

class SQLDB {
private:
	sqlite3 *db = nullptr;

public:
	SQLDB() = default;

	bool Execute(const std::string &code, void *callback = nullptr, void *data = nullptr);

	void Open(const std::string &path);
	void Close();

	int64_t GetLastRow() const { if (!db) return 0; return sqlite3_last_insert_rowid(db); }
};