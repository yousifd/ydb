#include <stdio.h>
#include <inttypes.h>

#include "include/db.h"
#include "include/utils.h"


int main(int argc, char* argv[]) {
    Database db;
    init_db(&db, 0);
    const Column columns[] = {{1, COLUMN_TYPE_UINT_8, "id"}};
    insert_db_table(&db, "test", columns, 1, 10);

    uint8_t value = 123, value2 = 22;
    Row defined_rows[] = {&value, &value2};
    Error err = insert_db_rows(&db, "test", defined_rows, sizeof(defined_rows) / sizeof(defined_rows[0]));
    if (err.msg) {
        printf("%s\n", err.msg);
    }
    
    uint8_t lookup_value = 123;
    Row* rows = get_db_equal_rows(&db, "test", "id", &lookup_value);

    if (rows == NULL) {
        printf("ROW ID: %d does not exist!\n", lookup_value);
        return 1;
    }

    printf("ROW ID: %d\n", *(uint8_t*)rows[0].value);

    return 0;
}