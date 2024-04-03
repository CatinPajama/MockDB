#ifndef DATABASE_H
#define DATABASE_H

#define MAX_COLS 5
#define MAX_TOKENS 10
#define MAX_ROWS 100
#define MAX_STR 30
#define MAX_SELECTED_ROWS 5

extern char query[MAX_TOKENS][MAX_STR];
extern int token_count;

struct Row
{
  int key;
  char data[MAX_COLS][MAX_STR];
};

struct Table
{
  char file_name[MAX_STR];
  char columns[MAX_COLS][MAX_STR];
  struct Row *table[MAX_ROWS];
  int rows;
  int cols;
  int loaded;
};

struct Database
{
  struct Table *curr_table;
  int selectedRowsCount;
  struct Row *selectedRows[MAX_SELECTED_ROWS];
};


/*
Example:
CREATE minions id,name,age
*/
void handleCreate(struct Database *db);

/*
Loads the table to work with
*/
void handleLoad(struct Database *db);

/*
Example:
INSERT 1,bob,10
*/
void handleInsert(struct Database* db);

void handleDelete(struct Database *db);

/*
Eammple Table:
id,name,age
1,bob,10
2,kevin,12
3,stuart,15
4,kevin,10
*/

/*
SELECT age > 10

select rows
2,kevin,12
3,stuart,15
*/
int handleSelect(struct Database *db);

/*
Used to narrow the previous select introcuing extra constarint
Looks like:

SELECT age > 10
AND name = kevin

select rows
2,kevin,12
*/
int handleAnd(struct Database *db);
int handleOR(struct Database *db);

/*
Clears any previous selection
*/
void handleClear(struct Database *db);

/*
Used to print the previously selected rows
*/
void handlePrint(struct Database *db);

void handleSave(struct Database *db);

#endif