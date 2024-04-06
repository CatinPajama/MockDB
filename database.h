#ifndef DATABASE_H
#define DATABASE_H

#define MAX_COLS 20
#define MAX_TOKENS 40
#define MAX_ROWS 100
#define MAX_STR 100
#define MAX_SELECTED_ROWS 500

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

struct Query {
  char data[MAX_TOKENS][MAX_STR];
  int query_len;
};

struct Database
{
  struct Table *curr_table;
  int selectedRowsCount;
  struct Row *selectedRows[MAX_SELECTED_ROWS];
  struct Query *query;
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

selected rows
2,kevin,12
3,stuart,15

Works with AND and OR too
as SELECT age > 10 AND name = kevin return 2,kevin,12 only
*/
int handleQuery(struct Database *db);

/*
Clears any previous selection
*/
void handleClear(struct Database *db);

/*
Used to print the previously selected rows
*/
void handlePrint(struct Database *db);

void handleSave(struct Database *db);

void handleOrderBy(struct Database *db, int offset_index);

void handleSchema(struct Database *db);
#endif
