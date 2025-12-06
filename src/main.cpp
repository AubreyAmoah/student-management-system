#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include "core/database.h"
#include "core/student.h"

int main(int argc, char** argv)
{
    try {
        // Create data directory if it doesn't exist
#ifdef _WIN32
        mkdir("data");
#else
        mkdir("data", 0755);
#endif

        // Initialize database
        Database db("data/students.db");

        // Create students table if it doesn't exist
        db.execute(R"(
            CREATE TABLE IF NOT EXISTS students (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                age INTEGER,
                grade TEXT
            )
        )");

        std::cout << "Database initialized successfully!" << std::endl;

    } catch (const DatabaseException& e) {
        std::cerr << "Database errorr: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}