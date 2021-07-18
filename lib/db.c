#include <memory.h>
#include <inttypes.h>

#include "../include/db.h"

void init_db(Database* db, size_t num_tables) {
    db->used_tables = 0;
    db->allocated_tables = num_tables;
    db->tables = NULL;

    if (num_tables > 0) {
        db->tables = malloc(sizeof(Table) * num_tables);
    }

    init_hashmap(&db->table_lookup, sizeof(Table*), 10);
}

void insert_db_table(Database* db, const char* name, const Column* columns, size_t num_columns, size_t num_rows) {
    if (db->used_tables + 1 >= db->allocated_tables) {
        Table** old_tables = &db->tables;
        const size_t new_size = db->allocated_tables == 0 ? 2 : db->allocated_tables * 2;

        db->tables = malloc(sizeof(Table) * new_size);
        if (old_tables != NULL) {
            memcpy(db->tables, *old_tables, sizeof(Table) * db->allocated_tables);
        }

        db->allocated_tables = new_size;
    }

    Table* table = &db->tables[db->used_tables];
    table->name = name;

    table->used_rows = 0;
    table->allocated_rows = num_rows;
    table->rows = NULL;

    table->schema.size = 0;
    table->schema.num_columns = num_columns;
    table->schema.columns = NULL;
    init_hashmap(&table->schema.columns_lookup, sizeof(Column*), 10);

    if (columns != NULL && num_columns > 0) {
        table->schema.columns = malloc(sizeof(Column) * num_columns);
        for (unsigned i = 0; i < num_columns; i++) {
            table->schema.columns[i].size = columns[i].size;
            table->schema.columns[i].type = columns[i].type;
            table->schema.columns[i].name = columns[i].name;
            table->schema.columns[i].offset = table->schema.size;

            table->schema.size += (size_t)columns[i].size;

            insert_hashmap(&table->schema.columns_lookup, columns[i].name, &table->schema.columns[i]);
        }

        if (num_rows > 0) {
            table->rows = malloc(sizeof(Row) * num_rows);
            for (unsigned i = 0; i < num_rows; i++) {
                table->rows[i].value = malloc(table->schema.size);
            }
        }
    }

    insert_hashmap(&db->table_lookup, name, &db->tables[db->used_tables]);

    db->used_tables += 1;
}

Error insert_db_rows(Database* db, const char* table_name, Row* rows, size_t num_rows) {
    Table* table = get_hashmap_value(&db->table_lookup, table_name, Table);
    if (table == NULL) {
        return (Error){"Failed to get table!"};
    }

    if (table->used_rows + num_rows >= table->allocated_rows) {
        Row** old_rows = &table->rows;
        size_t new_size = table->allocated_rows == 0 ? 1 : table->allocated_rows;
        while(new_size <= table->allocated_rows + num_rows) {
            new_size *= 2;
        }
        
        table->rows = malloc(sizeof(Row) * new_size);
        if (old_rows != NULL) {
            memcpy(table->rows, *old_rows, sizeof(Row) * table->allocated_rows);
        }

        for (unsigned i = table->used_rows; i < new_size; i++) {
            table->rows[i].value = malloc(table->schema.size);
        }

        table->allocated_rows = new_size;
    }

    for (unsigned i = 0; i < num_rows; i++) {
        memcpy(table->rows[table->used_rows + i].value, rows[i].value, table->schema.size);
    }

    table->used_rows += num_rows;

    return (Error){};
}

Row* get_db_equal_rows(Database* db, const char* table_name, const char* column_name, void* value) {
    Table* table = get_hashmap_value(&db->table_lookup, table_name, Table);
    if (table == NULL) {
        return NULL;
    }

    // TODO(yousifd): Query based on arbitrary columns and conditions

    Column* column = get_hashmap_value(&table->schema.columns_lookup, column_name, Column);
    if (column == NULL) {
        return NULL;
    }

    // TODO(yousifd): Replace this lookup with a b-tree
    switch (column->type) {
        case COLUMN_TYPE_UINT_8: {
            for (unsigned i = 0; i < table->used_rows; i++) {
                if (*(uint8_t*)(table->rows[i].value + column->offset) == *(uint8_t*)value) {
                    return &table->rows[i];
                }
            }
            break;
        } case COLUMN_TYPE_UINT_16: {
            for (unsigned i = 0; i < table->used_rows; i++) {
                if (*(uint16_t*)(table->rows[i].value + column->offset) == *(uint16_t*)value) {
                    return &table->rows[i];
                }
            }
            break;
        } case COLUMN_TYPE_UINT_32: {
            for (unsigned i = 0; i < table->used_rows; i++) {
                if (*(uint32_t*)(table->rows[i].value + column->offset) == *(uint32_t*)value) {
                    return &table->rows[i];
                }
            }
            break;
        }
    }

    return NULL;
}
