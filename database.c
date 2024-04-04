#include <stdio.h>
#include "database.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static int load_table(struct Database* db, char *file_name) {
  struct Table* table = db->curr_table;
  FILE* file = fopen(file_name,"r");
  if(!file){
    printf("Failed To Load File\n");
    return -1;
}
  strcpy(table->file_name,file_name);
  char row[MAX_COLS*MAX_STR+1];
  int row_no=0;

  // Resetting the rows and column to 0 before adding another table to current row
  // This is vital when we load another table when one table is already loaded
  table->rows = 0;
  table->cols = 0;
  while(fgets(row,sizeof(row),file)) {
    char temp_row[MAX_COLS*MAX_STR+1];
    strcpy(temp_row,row);
    temp_row[strcspn(temp_row, "\n")] = '\0';
    char *token;
    token = strtok(temp_row, " , ");
    int cols = 0;
    table->table[row_no] = malloc(sizeof(struct Row));
    // printf("%s \n",row);
    while (token != NULL)
    {
      if (row_no == 0)
      {
        strcpy(table->columns[cols], token);
      }
      else
      {
        strcpy(table->table[row_no]->data[cols], token);
      }
      token = strtok(NULL, " , ");
      cols++;
    }
    table->table[row_no]->key = row_no;
    db->curr_table->rows++;
    row_no++;
    table->cols = cols;
  }
  fclose(file);
  table->loaded = 1;
  return 1;
}

static int isNumber(char *str)
{
  for (int i = 0; i < strlen(str) - 1; i++)
  {
    if (!isdigit(str[i]))
    {
      return 0;
    }
  }
  return 1;
}

static int rowSatisfiesNumber(struct Row *row, char *val, int col, char *op)
{
  int row_val = atoi(row->data[col]);
  int ival = atoi(val);
  if (strcmp(op, "<") == 0)
  {
    return row_val < ival;
  }
  else if (strcmp(op, "=") == 0)
  {
    return row_val == ival;
  }
  else if (strcmp(op, ">") == 0)
  {
    return row_val > ival;
  }
  else if (strcmp(op, "*") == 0)
  {
    return 1;
  }
  return 1;
}
static int rowSatisfiesString(struct Row *row, char *val, int col, char *op)
{
  char *row_val = row->data[col];
  if (strcmp(op, "=") == 0)
  {
    return strcmp(row_val, val) == 0;
  }
  else if (strcmp(op, "!=") == 0)
  {
    return strcmp(row_val, val) != 0;
  }
  else if (!strcmp(op, "includes"))
  {
    return strstr(row_val, val) != NULL;
  }
  else if (!strcmp(op, "!includes"))
  {
    return strstr(row_val, val) == NULL;
  }
  else if (strcmp(op, "*") == 0)
  {
    return 1;
  }
  return 0;
}

int handleAnd(struct Database *db, int offset_index)
{
  
  struct Table *table = db->curr_table;
  char *col = db->query->data[offset_index+1];
  char *op = db->query->data[offset_index+2];
  char *val = db->query->data[offset_index+3];

  int found = -1;
  for (int i = 0; i < MAX_COLS; i++)
  {
    if (strcmp(table->columns[i], col) == 0)
    {
      found = i;
      break;
    }
  }
  if (found == -1)
  {
      printf("Feild not Found\n");
    return 0;
  }
  int isnum=0;
  isnum += isNumber(val);
  int length = db->selectedRowsCount;
  int r = 0;
  while(r < db->selectedRowsCount)
  {
    struct Row *row = db->selectedRows[r];
    int satisfies=0;
    if(isnum) satisfies=rowSatisfiesNumber(row, val, found, op);
    else satisfies = rowSatisfiesString(row,val,found,op);
    if (!satisfies)
    {
      db->selectedRowsCount--;
      for (int i = r; i < db->selectedRowsCount; i++)
      {
        db->selectedRows[i] = db->selectedRows[i + 1];
        // db->selectedRows[i]->key--;
      }
    } else {
      r++;
    }
  }
  printf("%d rows selected\n",db->selectedRowsCount);
  return 1;
}


int handleSelect(struct Database *db) {
  // Returns the number of query tokens used if successful
  // Else return 0 or negative number;

  struct Table* table = db->curr_table;
  if (!table->loaded) {
    printf("No Table Loaded...\n");
    return -1;
  }
  char *col = db->query->data[1];

  if (strcmp(col, "*") == 0) {
    db->selectedRowsCount = 0;
    for (int row_no = 1; row_no < table->rows; row_no++)
    {
      struct Row *row = table->table[row_no];
      db->selectedRows[db->selectedRowsCount++] = row;
    }
    printf("%d rows selected\n",db->selectedRowsCount);
    return 2;
  }

  char *op = db->query->data[2];
  char *val = db->query->data[3];

  db->selectedRowsCount = 0;
  int found = -1;
  for (int i = 0; i < MAX_COLS; i++)
  {
     //printf("columns are : %s", table->columns[i]);
    if (strcmp(table->columns[i], col) == 0)
    {
      found = i;
      break;
    }
  }
  if (found == -1)
  {
      printf("Feild not Found\n");
    return 0;
  }
  if (isNumber(val))
  {
    for (int row_no = 1; row_no < table->rows; row_no++)
    {
      struct Row *row = table->table[row_no];
      if (rowSatisfiesNumber(row, val, found, op))
      {
        db->selectedRows[db->selectedRowsCount++] = row;
      }
    }
    /*
    for(int i = 0; i < db->selectedRowsCount; i++) {
      printf("%s %d\n",db->selectedRows[i]->data[0],db->selectedRows[i]->key);
    }
    */
  }
  else
  {
    // HANDLE STRING TODO
    for (int row_no = 1; row_no < table->rows; row_no++)
    {
      struct Row *row = table->table[row_no];
      if (rowSatisfiesString(row, val, found, op))
      {
        db->selectedRows[db->selectedRowsCount++] = row;
      }
    }
  }
  printf("%d rows selected\n",db->selectedRowsCount);
  return 4;
}

int handleOR(struct Database *db, int offset_index)
{
  //printf("%s %s %s %s",operator,col,op,val);
  struct Table *table = db->curr_table;
  char *col = db->query->data[offset_index+1];
  char *op = db->query->data[offset_index+2];
  char *val = db->query->data[offset_index+3];

  int found = -1;
  for (int i = 0; i < MAX_COLS; i++)
  {
    // printf("columns are : %s", table->columns[i]);
    if (strcmp(table->columns[i], col) == 0)
    {
      found = i;
      break;
    }
  }
  if (found == -1)
  {
    printf("Feild not Found\n");
    return 0;
  }
  int isnum = isNumber(val);
  for (int row_no = 1; row_no < db->curr_table->rows; row_no++)
  {
    struct Row *row = table->table[row_no];
    int exists = -1;
    for (int i = 0; i < db->selectedRowsCount; i++)
    {
      if (row_no == db->selectedRows[i]->key)
      {
        exists = i;
        break;
      }
    }

    if (exists != -1)
    {
      continue;
    }
    int satisfies=0;
    if(isnum) satisfies=rowSatisfiesNumber(row,val,found,op);
    else satisfies=rowSatisfiesString(row,val,found,op);
    if (satisfies)
    {
      db->selectedRows[db->selectedRowsCount++] = row;
    }
  }
  printf("%d rows selected\n",db->selectedRowsCount);
  return 1;
}

void handlePrint(struct Database *db) {
  if(!db->curr_table->loaded) {
    printf("No Table Loaded...\n");
    return;
  }
  if(db->selectedRowsCount<1){
      printf("%d rows selected\n",db->selectedRowsCount);
      return;
  }
  //for(int i=0;i<db->curr_table->cols;i++){
    //  printf("%s",db->curr_table->columns[i]);
  //}
  for(int i = 0; i < db->selectedRowsCount;i++) {
    for(int j = 0; j < MAX_COLS; j++) {
      printf("%s ", db->selectedRows[i]->data[j]);
    }
    printf("\n");
  }
}

void handleInsert(struct Database* db) {
  if(!db->curr_table->loaded) {
    printf("No Table Loaded...\n");
    return;
  }
  char *input = db->query->data[1];
  input[strcspn(input, "\n")] = '\0';
  char *token;
  token = strtok(input, " , ");
  int cols = 0;
  // printf("%s \n",row);
  db->curr_table->table[db->curr_table->rows] = malloc(sizeof(struct Row));
  db->curr_table->table[db->curr_table->rows]->key = db->curr_table->rows;
  while (token != NULL)
  {
    strcpy(db->curr_table->table[db->curr_table->rows]->data[cols], token);
    token = strtok(NULL, " , ");
    cols++;
  }
  db->curr_table->rows++;
}

void handleDelete(struct Database *db)
{
  // TODO optimise
  if(!db->curr_table->loaded) {
    printf("No Table Loaded...\n");
    return;
  }
  struct Table* table = db->curr_table;
  for(int i = 0; i < db->selectedRowsCount; i++) {
    struct Row* row = db->selectedRows[i];
    db->curr_table->rows--;
    int index = row->key;
    free(row);
    for (int j = index; j < table->rows; j++)
    {
      table->table[j] = table->table[j + 1];
      table->table[j]->key--;
    }
  }

  for (int i = 1; i < db->curr_table->rows; i++)
  {
    printf("Currently : %s ", db->curr_table->table[i]->data[0]);
  }
}

void handleClear(struct Database *db)
{
  db->selectedRowsCount = 0;
}


void handleSave(struct Database *db) {
  if(!db->curr_table->loaded) {
    printf("No Table Loaded...\n");
    return;
  }
  FILE* file = fopen(db->curr_table->file_name,"w+");
  for(int c = 0; c < db->curr_table->cols; c++) {
    fprintf(file,"%s,",db->curr_table->columns[c]);
  }
  fprintf(file, "\n");
  for (int row = 1; row < db->curr_table->rows; row++)
  {
    for (int c = 0; c < db->curr_table->cols; c++)
    {
      // printf("%s ", db->curr_table->table[row]->data[c]);
      fprintf(file, "%s,", db->curr_table->table[row]->data[c]);
    }
    fprintf(file, "\n");
    // printf("\n");
  }
  fclose(file);
}

void handleCreate(struct Database *db) {
  char* table_name = db->query->data[1];
  char* columns = db->query->data[2];
  struct Table* table = db->curr_table;
  char file_name[30];
  sprintf(file_name, "%s.csv", table_name);
  strcpy(table->file_name,file_name);
  
  char *token;
  token = strtok(columns," , ");
  int cols=0;
  while(token != NULL) {
    strcpy(table->columns[cols++],token);
    token = strtok(NULL," , ");
  }
  table->cols = cols;
  table->rows = 0;
  
  FILE* file = fopen(db->curr_table->file_name,"w+");
  for(int c = 0; c < db->curr_table->cols; c++) {
    fprintf(file,"%s,",db->curr_table->columns[c]);
  }
  fprintf(file,"\n");
  fclose(file);
  load_table(db, file_name);
}

void handleLoad(struct Database *db) {
  char* table_name = db->query->data[1];
  struct Table* table = db->curr_table;
  char file_name[30];
  sprintf(file_name, "%s.csv", table_name);
  load_table(db, file_name);
}


void handleOrderBy(struct Database *db, int offset_index) {
  char* col = db->query->data[offset_index+1];
  int found=-1;
  for(int i = 0; i < db->curr_table->cols; i++) {
    if(strcmp(db->curr_table->columns[i],col) == 0){
      found=i;
      break;
    }
  }
  //printf("%s %d",col,found);
  int isNum = isNumber(db->curr_table->table[0]->data[found]);
  for(int i = 0; i < db->selectedRowsCount; i++) {
    for(int j = i + 1; j < db->selectedRowsCount; j++) {
      int firstMore=0;
      if(!isNum) firstMore=strcmp(db->selectedRows[i]->data[found],db->selectedRows[j]->data[found]);
      else {
        int a = atoi(db->selectedRows[i]->data[found]);
        int b = atoi(db->selectedRows[j]->data[found]);
        if(a > b) firstMore = 1;
        else if(a < b)firstMore = -1;
      }
      if(firstMore>0){
        struct Row* temp = db->selectedRows[i];
        db->selectedRows[i] = db->selectedRows[j];
        db->selectedRows[j] = temp;
      }
    }
  }
}

void handleSchema(struct Database* db) {
  for(int i = 0; i < db->curr_table->cols; i++) {
    printf("%s ", db->curr_table->columns[i]);
  }
  printf("\n");
}

int handleQuery(struct Database* db) {
  int select_tokens = handleSelect(db);
  for(int i = select_tokens; i < db->query->query_len;) {
    if (strcmp(db->query->data[i], "AND") == 0) {
      handleAnd(db, i);
      i += 4;
    } else if (strcmp(db->query->data[i], "OR") == 0) {
      handleOR(db, i);
      i += 4;
    } else if (strcmp(db->query->data[i],"ORDERBY") == 0) {
      handleOrderBy(db, i);
      i += 2;
    }
  }
  return 1;
}


