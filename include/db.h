#ifndef _DB_H_
#define _DB_H_

#include <stdlib.h>

#include "hashmap.h"
#include "utils.h"

// TODO(yousifd): Support more column types
typedef enum {
    COLUMN_TYPE_UINT_8, COLUMN_TYPE_UINT_16, COLUMN_TYPE_UINT_32,
} ColumnType;

typedef struct {
    size_t size;
    ColumnType type;
    const char* name;
    unsigned offset;
} Column;

typedef struct {
    Column* columns;
    size_t num_columns;
    size_t size;

    HashMap columns_lookup;
} Schema;

typedef struct {
    void* value;
} Row;

typedef struct {
    const char* name;
    Schema schema;
    Row* rows;
    size_t used_rows;
    size_t allocated_rows;
} Table;

typedef struct {
    Table* tables;
    size_t used_tables;
    size_t allocated_tables;

    HashMap table_lookup;
} Database;

void init_db(Database* db, size_t num_tables);
void insert_db_table(Database* db, const char* name, const Column* columns, size_t num_columns, size_t num_rows);
Error insert_db_rows(Database* db, const char* table_name, Row* rows, size_t num_rows);
Row* get_db_equal_rows(Database* db, const char* table_name, const char* column_name, void* value);

#endif // _DB_H_