#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>
#include "sqlite3.h"

class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message) 
        : std::runtime_error(message) {}
};

class Database {
private:
    sqlite3* db;
    std::string dbPath;
    bool isOpen;

    // Helper function to check if database is open
    void checkOpen() const;

public:
    // Constructor & Destructor
    explicit Database(const std::string& path);
    ~Database();

    // Prevent copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Allow moving
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;

    // Database operations
    void open();
    void close();
    bool isConnected() const { return isOpen; }

    // Execute SQL without returning results (INSERT, UPDATE, DELETE, CREATE TABLE)
    void execute(const std::string& sql);

    // Execute SQL with parameters (prepared statement)
    void executeWithParams(const std::string& sql, 
                          const std::vector<std::string>& params);

    // Query with callback for each row
    void query(const std::string& sql, 
              std::function<void(sqlite3_stmt*)> callback);

    // Query with parameters and callback
    void queryWithParams(const std::string& sql,
                        const std::vector<std::string>& params,
                        std::function<void(sqlite3_stmt*)> callback);

    // Transaction support
    void beginTransaction();
    void commit();
    void rollback();

    // Utility functions
    int getLastInsertId();
    int getChangesCount();
    std::string getErrorMessage() const;

    // Get raw database handle (use with caution)
    sqlite3* getHandle() { return db; }
};

// RAII Transaction helper
class Transaction {
private:
    Database& db;
    bool committed;

public:
    explicit Transaction(Database& database);
    ~Transaction();

    void commit();
    void rollback();

    // Prevent copying and moving
    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;
    Transaction(Transaction&&) = delete;
    Transaction& operator=(Transaction&&) = delete;
};

#endif // DATABASE_H