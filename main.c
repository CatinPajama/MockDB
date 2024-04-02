#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define MAX_COLUMNS  5
#define MAX_TOKENS  10
#define MAX_ROWS  100
#define MAX_STR  30
#define MAX_SELECTED_ROWS 5

struct Table {
  char file_name[MAX_STR];
  char columns[MAX_COLUMNS][MAX_STR];
  char table[MAX_ROWS][MAX_COLUMNS][MAX_STR];
  int lst_row;
  int lst_col;
};

struct Database {
  struct Table *curr_table;
  int selectedRows[MAX_SELECTED_ROWS];
  int selectedRowsCount;
};

int load_table(struct Database* db, char *file_name) {
  struct Table* table = db->curr_table;
  FILE* file = fopen(file_name,"rw");
  strcpy(table->file_name,file_name);
  char row[MAX_COLUMNS*MAX_STR+1];
  int row_no=0;
  while(fgets(row,sizeof(row),file)) {
    char temp_row[MAX_COLUMNS*MAX_STR+1];
    strcpy(temp_row,row);
    temp_row[strcspn(temp_row, "\n")] = '\0';
    char *token;
    token = strtok(temp_row," , ");
    int cols=0;
    //printf("%s \n",row);
    while(token != NULL) {
      if(row_no==0) {
        //strcpy(db->curr_table->columns[cols],"hi");
        //printf("size is %d %d",sizeof(token),sizeof(table->columns[cols]));
        strcpy(table->columns[cols],token);
        //printf("%s",token); 


      } else {
        //printf("PUTTING %s %d %d",token,row_no,cols);
        strcpy(table->table[row_no][cols],token);
      }
      token = strtok(NULL," , ");
      table->lst_col++;
      cols++;
    }
    db->curr_table->lst_row++;
    row_no++;
  }
  fclose(file);
  return 1;
}



char query[MAX_TOKENS][MAX_STR];

int isNumber(char *str) {
  for(int i = 0; i < strlen(str) - 1; i++) {
    if(!isdigit(str[i])) {
      return 0;
    }
  }
  return 1;
}

int rowSatisfiesNumber(int row_no, char (*row)[MAX_STR], char *val, int col, char *op) {
  int row_val = atoi(row[col]);
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
  for(int i = 0; i < MAX_COLUMNS; i++) {
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
    for(int r = 0; r < db->selectedRowsCount ; r++) {
      int row_no = db->selectedRows[r];
      //printf("ROW : %d %d",row_no,r);
      if(rowSatisfiesNumber(row_no,table->table[row_no],val,found,op)) {
        //printf("MATCHES?");
        int exists=-1;
        for(int i = 0; i < db->selectedRowsCount; i++) {
          if(row_no == db->selectedRows[i]) {
            exists = i;
            break;
          }
        }
        if(exists!=-1) continue;
        db->selectedRows[db->selectedRowsCount++]=row_no; 
        //printf("MATCHES : %d\n",row_no);
      } else {
        //printf("HIT FALSE");
        int removed=-1;
        for(int i = 0; i < db->selectedRowsCount; i++) {
          if(row_no == db->selectedRows[i]) {
            removed = i;
            break;
          }
        }
        if(removed==-1) continue;
        // printf("REMOVED ROW NO : %d\n",row_no);
        db->selectedRowsCount--;
        for(int i = removed; i < db->selectedRowsCount; i++) {
          db->selectedRows[i] = db->selectedRows[i+1];
        }
        r--;
      }
    } 
  } else {
    // HANDLE STRING TODO
  }
  return 1;
}



int handleSelect(struct Database *db) {
  struct Table* table = db->curr_table;
  char *col = query[1];
  char *op = query[2];
  char *val = query[3];

  int found=-1;
  for(int i = 0; i < MAX_COLUMNS; i++) {
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
    for(int row_no = 1; row_no < MAX_ROWS; row_no++) {
      if(rowSatisfiesNumber(row_no,table->table[row_no],val,found,op)) {
        db->selectedRows[db->selectedRowsCount++]=row_no; 
      }
    } 
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
  for(int i = 0; i < MAX_COLUMNS; i++) {
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
    for(int row_no = 1; row_no < MAX_ROWS; row_no++) {
      int exists=-1;
      for(int i = 0; i < db->selectedRowsCount; i++) {
        if(row_no == db->selectedRows[i]) {
          exists = i;
          break;
        }
      }
      if(exists!=-1) continue;

      if(rowSatisfiesNumber(row_no,table->table[row_no],val,found,op)) {
        db->selectedRows[db->selectedRowsCount++]=row_no; 
      }
    } 
  } else {
    // HANDLE STRING TODO
  }
  return 1;
}

void handlePrint(struct Database *db) {
  for(int i = 0; i < db->selectedRowsCount;i++) {
    for(int j = 0; j < MAX_COLUMNS; j++) {
      printf("%s ", db->curr_table->table[db->selectedRows[i]][j]);
    }
    printf("\n");
  }
}

void handleInsert(struct Database* db) {
  char *input = query[1];
  input[strcspn(input, "\n")] = '\0';
  char *token;
  token = strtok(input," , ");
  int cols=0;
  //printf("%s \n",row);
  while(token != NULL) {
    strcpy(db->curr_table->table[db->curr_table->lst_row][cols],token);
    token = strtok(NULL," , ");
    cols++;
  }
}


void handleDelete(struct Database *db) {
  // TODO optimise
  struct Table* table = db->curr_table;
  for(int i = 0; i < db->selectedRowsCount; i++) {
    int row = db->selectedRows[i];
    db->curr_table->lst_row--;
    for(int j = row; j < table->lst_row; j++) {
      for(int k = 0; k < MAX_COLUMNS; k++) {
        strcpy(table->table[j][k],table->table[j+1][k]);
      }
    }
  }  
}

void handleClear(struct Database *db) {
  db->selectedRowsCount=0;
}


void handleSave(struct Database *db) {
  FILE* file = fopen(db->curr_table->file_name,"rw");
  printf("row and col is %d %d",db->curr_table->lst_row,db->curr_table->lst_col);
  for(int row = 0; row < db->curr_table->lst_row; row++) {
    for(int c = 0; c < db->curr_table->lst_col; c++) {
      printf("%s ",db->curr_table->table[row][c]);
      fprintf(file,"%s,",db->curr_table->table[row][c]);
    }
    fprintf(file,"\n");
    printf("\n");
  }
  fclose(file);
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
  }
  
  else {
    printf("BAD");
  }

  return 1;
}

int main() {
  struct Database mydb;
  struct Table table;
  mydb.curr_table = &table;
  mydb.selectedRowsCount=0;


  printf("Enter file name to enter as table : ");
  char *file_name[30];
  scanf("%s",file_name);
  getchar();
  //fgets(file_name,sizeof(file_name),stdin);
  load_table(&mydb,file_name);
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
