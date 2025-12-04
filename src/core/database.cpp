#include "database.h"
#include <iostream>

Database::Database(const std::string& path) 
    : db(nullptr), dbPath(path), isOpen(false) {
    open();
}

Database::~Database() {
    close();
}

Database::Database(Database&& other) noexcept
    : db(other.db), dbPath(std::move(other.dbPath)), isOpen(other.isOpen) {
    other.db = nullptr;
    other.isOpen = false;
}

Database& Database::operator=(Database&& other) noexcept {
    if (this != &other) {
        close();
        db = other.db;
        dbPath = std::move(other.dbPath);
        isOpen = other.isOpen;
        other.db = nullptr;
        other.isOpen = false;
    }
    return *this;
}

void Database::open() {
    if (isOpen) {
        return;
    }

    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        throw DatabaseException("Cannot open database: " + error);
    }
    isOpen = true;
}

void Database::close() {
    if (db && isOpen) {
        sqlite3_close(db);
        db = nullptr;
        isOpen = false;
    }
}

void Database::checkOpen() const {
    if (!isOpen || !db) {
        throw DatabaseException("Database is not open");
    }
}

void Database::execute(const std::string& sql) {
    checkOpen();

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        throw DatabaseException("SQL execution error: " + error);
    }
}

void Database::executeWithParams(const std::string& sql, 
                                 const std::vector<std::string>& params) {
    checkOpen();

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("Failed to prepare statement: " + 
                              std::string(sqlite3_errmsg(db)));
    }

    // Bind parameters (1-indexed)
    for (size_t i = 0; i < params.size(); ++i) {
        rc = sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            sqlite3_finalize(stmt);
            throw DatabaseException("Failed to bind parameter: " + 
                                  std::string(sqlite3_errmsg(db)));
        }
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("Execution failed: " + 
                              std::string(sqlite3_errmsg(db)));
    }
}

void Database::query(const std::string& sql, 
                    std::function<void(sqlite3_stmt*)> callback) {
    checkOpen();

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("Failed to prepare statement: " + 
                              std::string(sqlite3_errmsg(db)));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        callback(stmt);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("Query execution failed: " + 
                              std::string(sqlite3_errmsg(db)));
    }
}

void Database::queryWithParams(const std::string& sql,
                              const std::vector<std::string>& params,
                              std::function<void(sqlite3_stmt*)> callback) {
    checkOpen();

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("Failed to prepare statement: " + 
                              std::string(sqlite3_errmsg(db)));
    }

    // Bind parameters
    for (size_t i = 0; i < params.size(); ++i) {
        rc = sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            sqlite3_finalize(stmt);
            throw DatabaseException("Failed to bind parameter: " + 
                                  std::string(sqlite3_errmsg(db)));
        }
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        callback(stmt);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("Query execution failed: " + 
                              std::string(sqlite3_errmsg(db)));
    }
}

void Database::beginTransaction() {
    execute("BEGIN TRANSACTION;");
}

void Database::commit() {
    execute("COMMIT;");
}

void Database::rollback() {
    execute("ROLLBACK;");
}

int Database::getLastInsertId() {
    checkOpen();
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

int Database::getChangesCount() {
    checkOpen();
    return sqlite3_changes(db);
}

std::string Database::getErrorMessage() const {
    if (db) {
        return sqlite3_errmsg(db);
    }
    return "Database not initialized";
}

// Transaction RAII implementation
Transaction::Transaction(Database& database) 
    : db(database), committed(false) {
    db.beginTransaction();
}

Transaction::~Transaction() {
    if (!committed) {
        try {
            db.rollback();
        } catch (...) {
            // Suppress exceptions in destructor
        }
    }
}

void Transaction::commit() {
    db.commit();
    committed = true;
}

void Transaction::rollback() {
    db.rollback();
    committed = true;
}