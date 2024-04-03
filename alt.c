#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_COLS  5
#define MAX_TOKENS  10
#define MAX_ROWS  100
#define MAX_STR  30
#define MAX_SELECTED_ROWS 5



struct Row {
  int key;
  char data[MAX_COLS][MAX_STR];
};



struct Table {
  char file_name[MAX_STR];
  char columns[MAX_COLS][MAX_STR];
  struct Row *table[MAX_ROWS];
  int rows;
  int cols;
  int loaded;
};

struct Database {
  struct Table *curr_table;
  int selectedRowsCount;
  struct Row* selectedRows[MAX_SELECTED_ROWS];
};


int load_table(struct Database* db, char *file_name) {
  struct Table* table = db->curr_table;
  FILE* file = fopen(file_name,"rw");
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
    token = strtok(temp_row," , ");
    int cols=0;
    table->table[row_no] = malloc(sizeof(struct Row));
    //printf("%s \n",row);
    while(token != NULL) {
      if(row_no==0) {
        strcpy(table->columns[cols],token);
      } else {
        strcpy(table->table[row_no]->data[cols],token);
      }
      token = strtok(NULL," , ");
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



char query[MAX_TOKENS][MAX_STR];
int token_count = 0;

int isNumber(char *str) {
  for(int i = 0; i < strlen(str) - 1; i++) {
    if(!isdigit(str[i])) {
      return 0;
    }
  }
  return 1;
}

int rowSatisfiesNumber(struct Row *row, char *val, int col, char *op) {
  int row_val = atoi(row->data[col]);
  int ival = atoi(val);
  if(strcmp(op,"<") == 0) {
    return row_val < ival;
  } else if(strcmp(op,"=") == 0) {
    return row_val == ival;
  } else if(strcmp(op,">") == 0) {
    return row_val  > ival;
  }
  return 1;
}
int rowSatisfiesString(int row_no, char* row[], int val,int col, char *op) {
  // TODO
  return 0;
}
int handleAnd(struct Database *db) {
  struct Table* table = db->curr_table;
  char *col = query[1];
  char *op = query[2];
  char *val = query[3];

  int found=-1;
  for(int i = 0; i < MAX_COLS; i++) {
    if(strcmp(table->columns[i],col) == 0){
      found=i;
      break;
    }
  }
  if(found==-1) {
    return 0;
  }
  if(isNumber(val)) {
    int length = db->selectedRowsCount;
    for(int r = 0; r < length ; r++) {
      struct Row* row = db->selectedRows[r];
      if(!rowSatisfiesNumber(row,val,found,op)) {
        db->selectedRowsCount--;
        for(int i = r; i < db->selectedRowsCount; i++) {
          db->selectedRows[i] = db->selectedRows[i+1];
          //db->selectedRows[i]->key--;
        }
      }
    } 
    for(int i = 0; i < db->selectedRowsCount; i++) {
      printf("%s %d\n",db->selectedRows[i]->data[0],db->selectedRows[i]->key);
    }
  } else {
    // HANDLE STRING TODO
  }
  return 1;
}



int handleSelect(struct Database *db) {
  struct Table* table = db->curr_table;
  if (!table->loaded) {
    printf("No Table Loaded...\n");
    return -1;
  }
  char *col = query[1];
  char *op = query[2];
  char *val = query[3];

  int found=-1;
  for(int i = 0; i < MAX_COLS; i++) {
    //printf("columns are : %s", table->columns[i]);
    if(strcmp(table->columns[i],col) == 0){
      found=i;
      break;
    }
  }
  if(found==-1) {
    return 0;
  }
  if(isNumber(val)) {
    for(int row_no = 1; row_no < table->rows; row_no++) {
      struct Row* row = table->table[row_no];
      if(rowSatisfiesNumber(row,val,found,op)) {
        db->selectedRows[db->selectedRowsCount++]=row; 
      }
    } 
    /*
    for(int i = 0; i < db->selectedRowsCount; i++) {
      printf("%s %d\n",db->selectedRows[i]->data[0],db->selectedRows[i]->key);
    }
    */
  } else {
    // HANDLE STRING TODO
  }
  return 1;
}


int handleOR(struct Database *db) {
  struct Table* table = db->curr_table;
  char *col = query[1];
  char *op = query[2];
  char *val = query[3];

  int found=-1;
  for(int i = 0; i < MAX_COLS; i++) {
    //printf("columns are : %s", table->columns[i]);
    if(strcmp(table->columns[i],col) == 0){
      found=i;
      break;
    }
  }
  if(found==-1) {
    return 0;
  }
  if(isNumber(val)) {
    for(int row_no = 1; row_no < db->curr_table->rows; row_no++) {
      struct Row* row = table->table[row_no];
      int exists=-1;
      for(int i = 0; i < db->selectedRowsCount; i++) {
        if(row_no == db->selectedRows[i]->key) {
          exists = i;
          break;
        }
      }

      if(exists!=-1) {
        continue;
      }
      if(rowSatisfiesNumber(row,val,found,op)) {
        db->selectedRows[db->selectedRowsCount++]=row;
      } else {
      }
    } 
  } else {
    // HANDLE STRING TODO
  }
  return 1;
}

void handlePrint(struct Database *db) {
  if(!db->curr_table->loaded) {
    printf("No Table Loaded...\n");
    return;
  }
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
  char *input = query[1];
  input[strcspn(input, "\n")] = '\0';
  char *token;
  token = strtok(input," , ");
  int cols=0;
  //printf("%s \n",row);
  db->curr_table->table[db->curr_table->rows] = malloc(sizeof(struct Row));
  db->curr_table->table[db->curr_table->rows]->key = db->curr_table->rows;
  while(token != NULL) {
    strcpy(db->curr_table->table[db->curr_table->rows]->data[cols],token);
    token = strtok(NULL," , ");
    cols++;
  }
  db->curr_table->rows++;
}


void handleDelete(struct Database *db) {
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
    for(int j = index; j < table->rows; j++) {
      table->table[j] = table->table[j+1];
      table->table[j]->key--;
    }
  }  

  for(int i = 1; i < db->curr_table->rows; i++) {
    printf("Currently : %s ", db->curr_table->table[i]->data[0]);
  }
}

void handleClear(struct Database *db) {
  db->selectedRowsCount=0;
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
  fprintf(file,"\n");
  for(int row = 1; row < db->curr_table->rows; row++) {
    for(int c = 0; c < db->curr_table->cols; c++) {
      //printf("%s ", db->curr_table->table[row]->data[c]);
      fprintf(file,"%s,",db->curr_table->table[row]->data[c]);
    }
    fprintf(file,"\n");
    //printf("\n");
  }
  fclose(file);
}

void handleCreate(struct Database *db) {
  char* table_name = query[1];
  char* columns = query[2];
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
  char* table_name = query[1];
  struct Table* table = db->curr_table;
  char file_name[30];
  sprintf(file_name, "%s.csv", table_name);
  load_table(db, file_name);
}

int solve(struct Database *db) {
  struct Table* table = db->curr_table;
  char *operator = query[0];
  //printf("%s %s %s %s",operator,col,op,val);
  if(strcmp("SELECT",operator) == 0) {
    handleSelect(db);
  } else if(strcmp("AND",operator) == 0) {
    handleAnd(db);
  } else if(strcmp("OR",operator) == 0) {
    handleOR(db);
  } else if(strcmp("PRINT",operator) == 0) {
    handlePrint(db);
  } else if(strcmp("INSERT",operator) == 0) {
    handleInsert(db);
  } else if(strcmp("DELETE",operator) == 0) {
    handleDelete(db);
  } else if(strcmp("CLEAR",operator) == 0) {
    handleClear(db);
  } else if(strcmp("SAVE",operator) == 0) {
    handleSave(db);
  } else if(strcmp("CREATE", operator) == 0) {
    handleCreate(db);
  } else if(strcmp("LOAD", operator) == 0) {
    handleLoad(db);
  }
  
  else {
    printf("BAD");
  }

  return 1;
}

int main() {
  struct Database mydb;
  struct Table table;
  table.loaded = 0;
  mydb.curr_table = &table;
  mydb.selectedRowsCount=0;


  //printf("Enter file name to enter as table : ");
  //char file_name[30];
  //scanf("%s",file_name);
  //getchar();
  //strcpy(file_name,"test.txt");
  //fgets(file_name,sizeof(file_name),stdin);
  //load_table(&mydb,file_name);
  char input[MAX_TOKENS * MAX_STR];
  do {
    printf("=> ");
    fgets(input,sizeof(input),stdin);
    input[strcspn(input, "\n")] = '\0';
    char* token = strtok(input," ");
    int token_cnt=0;
    while(token != NULL) {
      strcpy(query[token_cnt],token);
      token = strtok(NULL," ");
      token_cnt++;
    }
    token_count = token_cnt;
    solve(&mydb);
  } while(strcmp(input,"QUIT") != 0);
  /*
  strcpy(query[0],"SELECT");
  strcpy(query[1],"age");
  strcpy(query[2],">");
  strcpy(query[3],"10");
  solve(&mydb);
  */
  
  /*
  for(int i = 0; i < mydb.selectedRowsCount; i++) {
    printf("%d ",mydb.selectedRows[i]);
  }
  strcpy(query[0],"AND");
  strcpy(query[1],"marks");
  strcpy(query[2],">");
  strcpy(query[3],"50");
  solve(&mydb);
  */
}
