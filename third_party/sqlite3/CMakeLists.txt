# Remove the add_subdirectory line and add SQLite source directly
set(SQLITE3_SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/sqlite3)

# Add SQLite as a library
add_library(sqlite3 STATIC
    ${SQLITE3_SOURCE_DIR}/sqlite3.c
)

target_include_directories(sqlite3 PUBLIC
    ${SQLITE3_SOURCE_DIR}
)

# Optional: disable some SQLite warnings
target_compile_options(sqlite3 PRIVATE
    -DSQLITE_ENABLE_COLUMN_METADATA
    -DSQLITE_ENABLE_FTS5
)