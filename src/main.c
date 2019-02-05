/*
 * Copyright (c) 2019 Amine Ben Hassouna <amine.benhassouna@gmail.com>
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any
 * person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without
 * limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice
 * shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

#define USER_FIRST_NAME_SIZE    (50)
#define USER_LAST_NAME_SIZE     (50)
#define USER_EMAIL_SIZE         (USER_FIRST_NAME_SIZE + USER_LAST_NAME_SIZE + 20)
#define USER_PASSWORD_SIZE      (20)

#define USER_INSER_QUERY_SIZE \
    (USER_FIRST_NAME_SIZE + USER_LAST_NAME_SIZE \
    + USER_EMAIL_SIZE + USER_PASSWORD_SIZE \
    + 100)


// User info structure
struct User
{
    char first_name[USER_FIRST_NAME_SIZE];
    char last_name[USER_LAST_NAME_SIZE];

    time_t birthday;

    char email[USER_EMAIL_SIZE];
    char password[USER_PASSWORD_SIZE];
};
typedef struct User User;


time_t date_to_timestamp(int day, int month, int year);
bool table_exists(sqlite3* db, const char* table_name);
bool create_user_table(sqlite3* db);
bool insert_user(sqlite3* db, User* user);
void display_users(sqlite3* db);


int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;

    // SQLite database variable
    sqlite3* db;
    int rc;

    // Open the SQLite database 'users.db' (Create a new file if it does not exists)
    rc = sqlite3_open("users.db", &db);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // If the 'user' table does not exists
    if(!table_exists(db, "user")) {
        // Create 'user' table
        if(!create_user_table(db)) {
            sqlite3_close(db);
            return 1;
        }
    }

    // Specify the number of users to be added
    int users_num = -1;
    do {
        printf("Please enter the number of users to be added: ");
        if(scanf("%d", &users_num) != 1) {
          fprintf(stderr, "Error, please try again!\n");
          scanf("%*c");
        }
    } while(users_num < 0);


    // Add users
    for(int i = 0; i < users_num; ++i) {

        User user;

        printf("Please enter the user (%d) first name (%d max): ", i+1, USER_FIRST_NAME_SIZE);
        scanf("%s", user.first_name);

        printf("Please enter the user (%d) last name (%d max): ", i+1, USER_LAST_NAME_SIZE);
        scanf("%s", user.last_name);

        printf("Please enter the user (%d) birthday (d/m/yyyy): ", i+1);
        int birthday_day, birthday_month, birthday_year;
        scanf("%d/%d/%d", &birthday_day, &birthday_month, &birthday_year);
        user.birthday = date_to_timestamp(birthday_day, birthday_month, birthday_year);

        printf("Please enter the user (%d) email (%d max): ", i+1, USER_EMAIL_SIZE);
        scanf("%s", user.email);

        printf("Please enter the user (%d) password (%d max): ", i+1, USER_PASSWORD_SIZE);
        scanf("%s", user.password);

        if(!insert_user(db, &user)) {
            printf("Re-try to add user (%d)", i+1);
            i--;
        }
    }

    putc('\n', stdout);


    // Display the list of users
    display_users(db);


    putc('\n', stdout);


    // Close the SQLite database
    sqlite3_close(db);

    return 0;
}


// Convert date to timestamp
time_t date_to_timestamp(int day, int month, int year)
{
    struct tm date = {0};
    date.tm_year = year - 1900; // Years since 1900
    date.tm_mon = month - 1;
    date.tm_mday = day;
    return mktime(&date);
}

// Check if a table exists
bool table_exists(sqlite3* db, const char* table_name)
{
    sqlite3_stmt* statement;
    int rc;

    const char* sql_table_list = "SELECT name FROM sqlite_master WHERE type='table'";

    rc = sqlite3_prepare_v2(db, sql_table_list, strlen(sql_table_list), &statement, NULL);
    if(rc == SQLITE_OK) {
        // Loop through all the tables
        while(sqlite3_step(statement) == SQLITE_ROW) {
            if(!strcmp((const char*) sqlite3_column_text(statement, 0), table_name))
                return true;
        }
    }

    return false;
}

// Create 'user' table
bool create_user_table(sqlite3* db)
{
    sqlite3_stmt* statement;
    int rc;

    // SQL query to create the 'user' table
    const char* sql_create_user_table = "CREATE TABLE user ("
                                        "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
                                        "first_name TEXT NOT NULL,"
                                        "last_name TEXT NOT NULL,"
                                        "birthday INTEGER,"
                                        "email TEXT,"
                                        "password TEXT);";

    // Execute the previous query to create the 'user' table
    rc = sqlite3_prepare_v2(db, sql_create_user_table, strlen(sql_create_user_table), &statement, NULL);
    if(rc == SQLITE_OK) {
        rc = sqlite3_step(statement);
        sqlite3_finalize(statement);

    } else {
        fprintf(stderr, "Can't initialize the database: %s\n", sqlite3_errmsg(db));
        return false;
    }

    return true;
}

bool insert_user(sqlite3* db, User* user)
{
    sqlite3_stmt* statement;
    int rc;

    // Prepare the SQL query to insert the new user
    char sql_insert_user[USER_INSER_QUERY_SIZE];
    sprintf(sql_insert_user,
            "INSERT INTO user (first_name, last_name, birthday, email, password) VALUES ('%s', '%s', %lu, '%s', '%s')",
            user->first_name,
            user->last_name,
            (unsigned long) user->birthday,
            user->email,
            user->password);

    // Execute the previous query to insert a new line into the 'user' table
    rc = sqlite3_prepare_v2(db, sql_insert_user, strlen(sql_insert_user), &statement, NULL);
    if(rc == SQLITE_OK) {
        rc = sqlite3_step(statement);

    } else {
        fprintf(stderr, "Can't insert the user '%s %s': %s\n",
                user->first_name, user->last_name, sqlite3_errmsg(db));
        return false;
    }

    sqlite3_finalize(statement);

    return true;
}

void display_users(sqlite3* db)
{
    sqlite3_stmt* statement;
    int rc;

    const char* sql_display_users = "SELECT first_name, last_name, birthday FROM user";

    // Execute the previous query to select all user in the 'user' table
    rc = sqlite3_prepare_v2(db, sql_display_users, strlen(sql_display_users), &statement, NULL);
    if(rc == SQLITE_OK) {

        // Display table header
        printf("%10s | %10s | %10s\n", "first_name", "last_name", "birthday");
        printf("------------------------------------\n");

        while(sqlite3_step(statement) == SQLITE_ROW) {

            User user = {0};

            // Loop through all the selected columns : first_name (i=0), last_name (i=1), birthday (i=3)
            for(int i = 0; i < sqlite3_column_count(statement); ++i) {

                if(sqlite3_column_type(statement, i) == SQLITE_NULL) {

                    switch(i) {
                    case 0: // first_name
                        strcpy(user.first_name, "(null)");
                        break;

                    case 1: // last_name
                        strcpy(user.last_name, "(null)");
                        break;

                    case 2: // birthday
                        user.birthday = 0;
                        break;
                    }
                } else {

                    switch(i) {
                    case 0: // first_name
                        strcpy(user.first_name, (const char*) sqlite3_column_text(statement, i));
                        break;

                    case 1: // last_name
                        strcpy(user.last_name, (const char*) sqlite3_column_text(statement, i));
                        break;

                    case 2: // birthday
                        user.birthday = sqlite3_column_int(statement, i);
                        break;
                    }
                }
            }

            // Convert timestamp to local datetime
            struct tm* birthday = localtime(&user.birthday);

            // Display user info
            printf("%10s | %10s | %02d/%02d/%04d\n",
                   user.first_name, user.last_name,
                   birthday->tm_mday, birthday->tm_mon + 1, birthday->tm_year + 1900);
        }
    }

    sqlite3_finalize(statement);
}
