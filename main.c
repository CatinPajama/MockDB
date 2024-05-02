#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "database.h"

//char query[MAX_TOKENS][MAX_STR];
int token_count = 0;
void handleHelp(){
    FILE *file = fopen("helpdoc.txt","r");
    if (!file)
      return;

    do
    {
        char c = fgetc(file);
        if (feof(file))
            break ;
        printf("%c", c);
    }  while(1);

}
int solve(struct Database *db) {
  struct Table* table = db->curr_table;
  char *operator = db->query->data[0];
  //printf("%s %s %s %s",operator,col,op,val);
  //handleQuery(db);
  if (strcmp("SELECT", operator) == 0)
  {
    handleQuery(db);
  } else if (strcmp("PRINT", operator) == 0)
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
  }
  else if(strcmp("LOAD", operator) == 0) {
    handleLoad(db);
  }
  else if(strcmp("SCHEMA", operator) == 0) {
    handleSchema(db);
  }
  else if(strcmp("HELP", operator) == 0) {
    handleHelp();
  }
  else if(strcmp("QUIT", operator) == 0) {}
  else{
    printf("BAD: type HELP\n");
  }


//  strcpy(query[0], "");
  return 1;
}

int main()
{
  struct Database mydb;
  struct Table table;
  struct Query query;
  table.loaded = 0;
  query.query_len=0;
  mydb.curr_table = &table;
  mydb.selectedRowsCount = 0;
  mydb.query = &query;

  char input[MAX_TOKENS * MAX_STR];
  do
  {
    printf("=> ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    char *token = strtok(input, " ");
    int token_cnt = 0;
    mydb.query->query_len=0;
    while (token != NULL)
    {
      strcpy(mydb.query->data[token_cnt], token);
      token = strtok(NULL, " ");
      token_cnt++;
      mydb.query->query_len++;
    }
    token_count = token_cnt;
    solve(&mydb);
  } while (strcmp(input, "QUIT") != 0);
}
