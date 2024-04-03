#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "database.h"

char query[MAX_TOKENS][MAX_STR];
int token_count = 0;

int solve(struct Database *db) {
  struct Table* table = db->curr_table;
  char *operator = query[0];
  //printf("%s %s %s %s",operator,col,op,val);
  if(strcmp("SELECT",operator) == 0) {
    handleSelect(db);
  }
  else if (strcmp("AND", operator) == 0)
  {
    handleAnd(db);
  }
  else if (strcmp("OR", operator) == 0)
  {
    handleOR(db);
  }
  else if (strcmp("PRINT", operator) == 0)
  {
    handlePrint(db);
  }
  else if (strcmp("INSERT", operator) == 0)
  {
    handleInsert(db);
  }
  else if (strcmp("DELETE", operator) == 0)
  {
    handleDelete(db);
  }
  else if (strcmp("CLEAR", operator) == 0)
  {
    handleClear(db);
  }
  else if (strcmp("SAVE", operator) == 0)
  {
    handleSave(db);
  } else if(strcmp("CREATE", operator) == 0) {
    handleCreate(db);
  } else if(strcmp("LOAD", operator) == 0) {
    handleLoad(db);
  }

  else
  {
    printf("BAD");
  }
  strcpy(query[0], "");
  return 1;
}

int main()
{
  struct Database mydb;
  struct Table table;
  table.loaded = 0;
  mydb.curr_table = &table;
  mydb.selectedRowsCount = 0;

  char input[MAX_TOKENS * MAX_STR];
  do
  {
    printf("=> ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    char *token = strtok(input, " ");
    int token_cnt = 0;
    while (token != NULL)
    {
      strcpy(query[token_cnt], token);
      token = strtok(NULL, " ");
      token_cnt++;
    }
    token_count = token_cnt;
    solve(&mydb);
  } while (strcmp(input, "QUIT") != 0);
}
