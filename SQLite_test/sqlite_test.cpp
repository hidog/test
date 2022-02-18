#include "sqlite_test.h"

#include <sqlite3.h>
#include <stdio.h>
#include <random>


namespace sqlite_test_1 {


void create_data()
{
    sqlite3 *db = nullptr;
    int rc;

    rc = sqlite3_open( "test.db", &db );
    if( rc != SQLITE_OK )
        printf("error\n");

    // create main_table
    char str[1000], name[100];
    char *err_msg;

    sprintf( str, "create table if not exists test_1( nid integer not null primary key, name text not null, height real )" );

    rc = sqlite3_exec( db, str, 0, 0, &err_msg );
    if( rc != SQLITE_OK )
    {
        printf( "create table fail.\nmsg = %s\n", err_msg );
        sqlite3_free( err_msg );
    }

    // insert data
    std::random_device rd;
    std::normal_distribution<double> dist( 175.0, 15.0 );

    // 批量處理, 加上這行會讓效能大幅提升
    sqlite3_exec(db, "begin transaction;", NULL, NULL, NULL);

    for( int i = 0; i < 10000; i++ )
    {
        printf( "the %d insert...\n", i );

        sprintf( name, "The %d-th data.", i );
        sprintf( str, "insert or ignore into test_1( nid, name, height ) values( %d, '%s', %lf )", i, name, dist(rd) );

        rc = sqlite3_exec( db, str, 0, 0, &err_msg );
        if( rc != SQLITE_OK )
        {
            printf("add data fail. msg = %s", err_msg);
            break;
        }
    }

    sqlite3_exec(db, "end transaction;", NULL, NULL, NULL);

    // close table
    rc = sqlite3_close(db);
    if( rc != SQLITE_OK )
        printf("close fail.\n");
}



void create_data_bind()
{
    sqlite3 *db = nullptr;
    int rc, index = 0;
    double value;

    rc = sqlite3_open( "test.db", &db );
    if( rc != SQLITE_OK )
        printf("error\n");

    // create main_table
    char str[1000], name[100];
    char *err_msg;

    sprintf( str, "create table if not exists test_1( nid integer not null primary key, name text not null, height real )" );

    rc = sqlite3_exec( db, str, 0, 0, &err_msg );
    if( rc != SQLITE_OK )
    {
        printf( "create table fail.\nmsg = %s\n", err_msg );
        sqlite3_free( err_msg );
    }

    // insert data
    sqlite3_exec(db, "begin transaction;", NULL, NULL, NULL);

    std::random_device rd;
    std::normal_distribution<double> dist( 175.0, 15.0 );

    sqlite3_stmt *stmt = NULL;

    for( int i = 0; i < 10000; i++ )
    {
        printf( "the %d insert...\n", i );

        sprintf( name, "The %d-th data.", i );
        sprintf( str, "insert or ignore into test_1( nid, name, height ) values( :nid, :name, :height )" );

        rc = sqlite3_prepare_v2( db, str, strlen(str)+1, &stmt, 0 );
        if( rc != SQLITE_OK )
        {
            printf("prepare fail.\n");
            break;
        }

        //
        index = sqlite3_bind_parameter_index( stmt, ":nid" );
        rc = sqlite3_bind_int( stmt, index, i );
        if( rc != SQLITE_OK )
        {
            printf("bind nic fail.\n");
            break;
        }

        //
        index = sqlite3_bind_parameter_index( stmt, ":name" );
        rc = sqlite3_bind_text( stmt, index, name, strlen(name)+1, 0 );
        if( rc != SQLITE_OK )
        {
            printf("bind nic fail.\n");
            break;
        }

        //
        index = sqlite3_bind_parameter_index( stmt, ":height" );
        value = dist(rd);
        rc = sqlite3_bind_double( stmt, index, value );
        if( rc != SQLITE_OK )
        {
            printf("bind nic fail.\n");
            break;
        }

        //
        rc = sqlite3_step(stmt);
        if( rc != SQLITE_DONE )
        {
            printf("insert data fail. msg\n");
            break;
        }

        //
        sqlite3_finalize(stmt);
    }

    sqlite3_exec(db, "end transaction;", NULL, NULL, NULL);


    // close table
    rc = sqlite3_close(db);
    if( rc != SQLITE_OK )
        printf("close fail.\n");
}






void delete_data()
{
    sqlite3 *db = nullptr;
    int rc;

    rc = sqlite3_open( "test.db", &db );
    if( rc != SQLITE_OK )
        printf("error\n");

    // create main_table
    char str[1000];
    char *err_msg;

    // delete data
#if 0
    for( int i = 0; i < 10; i++ )
    {
        printf( "the %d delete...\n", i );
        sprintf( str, "delete from test_1 where nid = %d", i );

        rc = sqlite3_exec( db, str, 0, 0, &err_msg );
        if( rc != SQLITE_OK )
        {
            printf("delete proj fail. msg = %s", err_msg);
            break;
        }
    }
#else
    printf("delete data...\n");
    sprintf( str, "delete from test_1 where height > 170.5" );
    rc = sqlite3_exec( db, str, 0, 0, &err_msg );
    if( rc != SQLITE_OK )    
        printf("delete data fail. msg = %s\n", err_msg );           
#endif

    // close table
    rc = sqlite3_close(db);
    if( rc != SQLITE_OK )
        printf("close fail.\n");
}




void count_data()
{
    sqlite3 *db = nullptr;
    int rc;

    rc = sqlite3_open( "test.db", &db );
    if( rc != SQLITE_OK )
        printf("error\n");

    char str[1000];

    // count data.
    int count = 0;
    sprintf( str, "select count(*) from test_1 where height > 160.8");

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2( db, str, strlen(str)+1, &stmt, 0 );

    rc = sqlite3_step(stmt);
    if( rc == SQLITE_ROW )
        count = sqlite3_column_int( stmt, 0 );
    else if( rc == SQLITE_DONE )
        count = -1;
    else
        printf( "sqlite query fail.\n" );

    sqlite3_finalize(stmt);

    // output result
    printf( "count = %d\n", count );

    // close table
    rc = sqlite3_close(db);
    if( rc != SQLITE_OK )
        printf("close fail.\n");
}




void select_data()
{
    sqlite3 *db = nullptr;
    int rc;

    rc = sqlite3_open( "test.db", &db );
    if( rc != SQLITE_OK )
        printf("error\n");

    char str[1000];

    // select data.
    sprintf( str, "select nid, name, height from test_1 order by height" );

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2( db, str, strlen(str)+1, &stmt, 0 );

    int nid, height;
    std::string name;

    while(true)
    {
        rc = sqlite3_step(stmt);
        if( rc == SQLITE_ROW )
        {
            nid = sqlite3_column_int( stmt, 0 );
            name = (char*)sqlite3_column_text( stmt, 1 );
            height = sqlite3_column_double( stmt, 2 );

            printf("nid = %3d, name = %10s, height = %3d\n", nid, name.c_str(), height );
        }
        else if( rc == SQLITE_DONE )
            break;
        else
            printf( "db error\n" );
    }

    sqlite3_finalize(stmt);

    // close table
    rc = sqlite3_close(db);
    if( rc != SQLITE_OK )
        printf("close fail.\n");
}





} // end sqlite_test_1





namespace sqlite_test_2 {



void create_data()
{
    sqlite3 *db = nullptr;
    int rc;

    rc = sqlite3_open( "test.db", &db );
    if( rc != SQLITE_OK )
        printf("error\n");

    // create main_table
    char str[1000], name[100], pname[100];
    char *err_msg;

    sprintf( str, "create table if not exists music_table( track integer not null primary key, name text not null, data blob,\
                                                           filesize integer not null, pic blob, psize integer not null )" );

    rc = sqlite3_exec( db, str, 0, 0, &err_msg );
    if( rc != SQLITE_OK )
    {
        printf( "create table fail.\nmsg = %s\n", err_msg );
        sqlite3_free( err_msg );
    }

    // insert data
    FILE *p, *q;
    int filesize = 0, psize = 0;
    sqlite3_stmt *stmt;
    int index;
    unsigned char *data = NULL, *pic = NULL;

    for( int i = 1; i <= 13; i++ )
    {
        sprintf( name, "D:\\tmp\\CD Track %02d.mp3", i );

        p = fopen( name, "rb" );
        if( p == NULL )
        {
            printf("open file fail.\n");
            break;
        }

        fseek( p, 0, SEEK_END );
        filesize = ftell(p);
        fseek( p, 0, SEEK_SET );

        printf( "%s %d bytes\n", name, filesize );

        // load pic
        sprintf( pname, "D:\\tmp\\a (%d).jpg", i );

        q = fopen( pname, "rb" );
        if( p == NULL )
        {
            printf("open file fail.\n");
            break;
        }

        fseek( q, 0, SEEK_END );
        psize = ftell(q);
        fseek( q, 0, SEEK_SET );

        /*
            這邊示範了兩個不同的方式來insert blob資料
            一個是指定名稱, 可以用函數取得index
            一個是 ?, 不指定名稱, 但index並非table column排序, 固定1, 2.
            有機會在看有沒有文件解釋這邊index的設計
        */
#if 0
        sprintf( str, "insert into music_table values( %d, '%s', :music_data, %d, 0, 0 )", i, name, filesize );
#else
        sprintf( str, "insert into music_table values( %d, '%s', ?, %d, ?, %d )", i, name, filesize, psize );
#endif

        //
        rc = sqlite3_prepare_v2( db, str, strlen(str)+1, &stmt, 0 );
        if( rc != SQLITE_OK )
        {
            printf("prepare fail\n" );
            break;            
        }

        // load file.
        data = new unsigned char[filesize];
        fread( data, 1, filesize, p );

        pic = new unsigned char[psize];
        fread( pic, 1, psize, q );

#if 0
        index = sqlite3_bind_parameter_index( stmt, ":music_data" );
        rc = sqlite3_bind_blob( stmt, index, data, filesize, SQLITE_STATIC );
#else 
        rc = sqlite3_bind_blob( stmt, 1, data, filesize, SQLITE_STATIC );
        rc = sqlite3_bind_blob( stmt, 2, pic, psize, SQLITE_STATIC );
#endif  

        if( rc != SQLITE_OK )
        {
            printf("bind fail.\n");
            break;
        }

        // exec
        rc = sqlite3_step(stmt);
        if( rc != SQLITE_DONE )
        {
            printf("run fail\n");
            break;
        }

        fclose(p);
        p = NULL;
        delete [] data;
        data = NULL;

        sqlite3_finalize(stmt);
    }

    if( p != NULL )
        fclose(p);

    // close table
    rc = sqlite3_close(db);
    if( rc != SQLITE_OK )
        printf("close fail.\n");
}





void select_data()
{
    sqlite3 *db = nullptr;
    int rc;

    rc = sqlite3_open( "test.db", &db );
    if( rc != SQLITE_OK )
        printf("error\n");

    char str[1000];

    // select data.
    sprintf( str, "select track, data, filesize, pic, psize from music_table order by filesize" );

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2( db, str, strlen(str)+1, &stmt, 0 );

    int track, filesize, psize;
    char filename[1000];
    unsigned char *data = NULL, *pic = NULL;
    FILE *p = NULL;
    FILE *q = NULL;

    int idx = 0;

    while(true)
    {
        rc = sqlite3_step(stmt);
        if( rc == SQLITE_ROW )
        {
            track = sqlite3_column_int( stmt, 0 );
            data = (unsigned char*)sqlite3_column_blob( stmt, 1 );
            filesize = sqlite3_column_int( stmt, 2 );
            pic = (unsigned char*)sqlite3_column_blob( stmt, 3 );
            psize = sqlite3_column_int( stmt, 4 );

            // music
            sprintf( filename, "idx_%d_track_%d.mp3", idx, track );
            printf( "file %s\n", filename );
            p = fopen( filename, "wb+" );
            fwrite( data, 1, filesize, p );
            fclose(p);

            // pic
            if( psize > 0 )
            {
                sprintf( filename, "%d.jpg", idx );
                q = fopen( filename, "wb+" );
                fwrite( pic, 1, psize, q );
                fclose(q);
            }
        }
        else if( rc == SQLITE_DONE )
            break;
        else
            printf( "db error\n" );

        idx++;
    }

    sqlite3_finalize(stmt);

    // close table
    rc = sqlite3_close(db);
    if( rc != SQLITE_OK )
        printf("close fail.\n");
}



} // end sqlite_test_2
