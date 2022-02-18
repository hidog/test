#ifndef SQLITE_TEST_H
#define SQLITE_TEST_H


/*
https://www.1ju.org/sqlite/primary-key

https://www.sqlite.org/c3ref/bind_blob.html   
sqlite3_bind


sqlite3_column_blob()
sqlite3_column_bytes()
sqlite3_column_bytes16()
sqlite3_column_count()
sqlite3_column_double()
sqlite3_column_int()
sqlite3_column_int64()
sqlite3_column_text()
sqlite3_column_text16()
sqlite3_column_type()
sqlite3_column_value()

https://www.sqlite.org/c3ref/column_blob.html

*/



namespace sqlite_test_1 {

void create_data();
void create_data_bind();
void delete_data();
void count_data();
void select_data();

} // end sqlite_test_1

namespace sqlite_test_2 {

void create_data();
void select_data();

} // end sqlite_test_2


#endif