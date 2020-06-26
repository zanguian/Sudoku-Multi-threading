#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

FILE *path;

typedef int bool;

#define TRUE 1
#define FALSE 0

#define NUMBER_OF_CHILD 9

int sudokuPuzzle[NUMBER_OF_CHILD][NUMBER_OF_CHILD]; /* the actual puzzle itself */

bool columns[NUMBER_OF_CHILD];
bool rows[NUMBER_OF_CHILD];
bool subgrids[NUMBER_OF_CHILD];

void *colrunner(void *param);
void *rowrunner(void *param);
void *subgridrunner(void *param);

typedef struct {
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
}   index_range;

int main(){

    char fname[1000];

    printf("Please return the name of the sudoku text file.\n");

    scanf("%s", fname);

    path = fopen(fname, "r");
    
    if(path == NULL){
        printf("The file could not be opened.\n");
        exit(1);
    }

    int i;
    int j = 0;
    int k = 0;
    int number;

    bool valid = TRUE;

    while(fscanf(path, "%d", &number) != EOF){
        sudokuPuzzle[j][k] = number;
        k++;
        if(k == 9){
            k = 0;
            j++;
        }
    }
    fclose(path);

    pthread_t tid_column[NUMBER_OF_CHILD];
    pthread_t tid_row[NUMBER_OF_CHILD];
    pthread_t tid_subgrid[NUMBER_OF_CHILD];

    index_range colstructs[NUMBER_OF_CHILD];
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        colstructs[i].topRow = 0;
        colstructs[i].bottomRow = 8;
        colstructs[i].leftColumn = i;
        colstructs[i].rightColumn = i;
    }
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        pthread_create(&(tid_column[i]), NULL, colrunner, &(colstructs[i]));
    }

    index_range rowstructs[NUMBER_OF_CHILD];
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        rowstructs[i].topRow = i;
        rowstructs[i].bottomRow = i;
        rowstructs[i].leftColumn = 0;
        rowstructs[i].rightColumn = 8;
    }
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        pthread_create(&(tid_row[i]), NULL, rowrunner, &(rowstructs[i]));
    }

    index_range subgridstructs[NUMBER_OF_CHILD];
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        if(i < 3){
            subgridstructs[i].topRow = 0;
            subgridstructs[i].bottomRow = 2;
        }
        else if(i > 2 && i < 6){
            subgridstructs[i].topRow = 3;
            subgridstructs[i].bottomRow = 5;
        }
        else if(i > 5 && i < NUMBER_OF_CHILD){
            subgridstructs[i].topRow = 6;
            subgridstructs[i].bottomRow = 8;
        }
        if(i == 0 || i == 3 || i == 6){
            subgridstructs[i].leftColumn = 0;
            subgridstructs[i].rightColumn = 2; 
        }
        else if(i == 1 || i == 4 || i == 7){
            subgridstructs[i].leftColumn = 3;
            subgridstructs[i].rightColumn = 5; 
        }
        else if(i == 2 || i == 5 || i == 8){
            subgridstructs[i].leftColumn = 6;
            subgridstructs[i].rightColumn = 8; 
        }
    }
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        pthread_create(&(tid_subgrid[i]), NULL, subgridrunner, &(subgridstructs[i]));
    }

    for(i = 0; i < NUMBER_OF_CHILD; i++){
        pthread_join(tid_column[i], NULL);
    }
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        pthread_join(tid_row[i], NULL);
    }
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        pthread_join(tid_subgrid[i], NULL);
    }

    for(i = 0; i < NUMBER_OF_CHILD; i++){
        if(columns[i] == TRUE){
            printf("Column: %lX valid\n", (unsigned long)tid_column[i]);
        }
        else if(columns[i] == FALSE){
            printf("Column: %lX invalid\n", (unsigned long)tid_column[i]);
            valid = FALSE;
        }
    }
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        if(rows[i] == TRUE){
            printf("Row: %lX valid\n", (unsigned long)tid_row[i]);
        }
        else if(rows[i] == FALSE){
            printf("Row: %lX invalid\n", (unsigned long)tid_row[i]);
            valid = FALSE;
        }
    }
    for(i = 0; i < NUMBER_OF_CHILD; i++){
        if(subgrids[i] == TRUE){
            printf("Subgrid: %lX valid\n", (unsigned long)tid_subgrid[i]);
        }
        else if(subgrids[i] == FALSE){
            printf("Subgrid: %lX invalid\n", (unsigned long)tid_subgrid[i]);
            valid = FALSE;
        }
    }
    if(valid == TRUE){
        printf("Sudoku Puzzle: valid\n");
    } else if(valid == FALSE){
        printf("Sudoku Puzzle: invalid\n");
    }
}

void *colrunner(void *param){
    int requiredNums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    index_range *ir;
    int i;
    int z;
    int topR, bottomR, leftC, rightC;
    pthread_t self;

    ir = (index_range *)param;
    topR = ir->topRow;
    bottomR = ir->bottomRow;
    leftC = ir->leftColumn;
    rightC = ir->rightColumn;

    int singleFound = 0;
    int count = 0;

    self = pthread_self();

    for(i = 0; i < NUMBER_OF_CHILD; i++){
        singleFound = 0;
        for(z = 0; z < 9; z++){
            if(sudokuPuzzle[i][leftC] == 0){
                goto decision;
            }
            if(sudokuPuzzle[i][leftC] == requiredNums[z]){
                requiredNums[z] = 0;
                singleFound = 1;
                count++;
                break;  
            }
        }
        if(singleFound == 0){
            goto decision;
        }
    }
decision:
    if(count != NUMBER_OF_CHILD){
        columns[leftC] = FALSE;
        printf("%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n", (unsigned
            long)self, topR, bottomR, leftC, rightC);
    }
    else if(count == NUMBER_OF_CHILD){
        columns[leftC] = TRUE;
        printf("%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n", (unsigned
            long)self, topR, bottomR, leftC, rightC);
    }
    pthread_exit(0);
}

void *rowrunner(void *param){
    int requiredNums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    index_range *ir;
    int i;
    int z;
    int topR, bottomR, leftC, rightC;
    pthread_t self;

    ir = (index_range *)param;
    topR = ir->topRow;
    bottomR = ir->bottomRow;
    leftC = ir->leftColumn;
    rightC = ir->rightColumn;

    int singleFound = 0;
    int count = 0;

    self = pthread_self();

    for(i = 0; i < NUMBER_OF_CHILD; i++){
        singleFound = 0;
        for(z = 0; z < 9; z++){
            if(sudokuPuzzle[topR][i] == 0){
                goto decision;
            }
            if(sudokuPuzzle[topR][i] == requiredNums[z]){
                requiredNums[z] = 0;
                singleFound = 1;
                count++;
                break;  
            }
        }
        if(singleFound == 0){
            goto decision;
        }
    }
decision:
    if(count != NUMBER_OF_CHILD){
        rows[topR] = FALSE;
        printf("%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n", (unsigned
            long)self, topR, bottomR, leftC, rightC);
    }
    else if(count == NUMBER_OF_CHILD){
        rows[topR] = TRUE;
        printf("%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n", (unsigned
            long)self, topR, bottomR, leftC, rightC);
    }
    pthread_exit(0);
}

void *subgridrunner(void *param){
    int requiredNums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    index_range *ir;
    int i;
    int j;
    int z;
    int topR, bottomR, leftC, rightC;
    int sub;
    pthread_t self;

    ir = (index_range *)param;
    topR = ir->topRow;
    bottomR = ir->bottomRow;
    leftC = ir->leftColumn;
    rightC = ir->rightColumn;

    int singleFound = 0;
    int count = 0;

    self = pthread_self();

    for(i = topR; i <= bottomR; i++){
        for(j = leftC; j <= rightC; j++){
            singleFound = 0;
            for(z = 0; z < 9; z++){
                if(sudokuPuzzle[i][j] == 0){
                    goto decision;
                }
                if(sudokuPuzzle[i][j] == requiredNums[z]){
                    requiredNums[z] = 0;
                    singleFound = 1;
                    count++;
                    break;  
                }
            }
            if(singleFound == 0){
                goto decision;
            }
        }
    }
decision:
    if(topR == 0){
        if(leftC == 0){
            sub = 0;
        } else if(leftC == 3){
            sub = 1;
        } else if(leftC == 6){
            sub = 2;
        }
    } else if (topR == 3){
        if(leftC == 0){
            sub = 3;
        } else if(leftC == 3){
            sub = 4;
        } else if(leftC == 6){
            sub = 5;
        }
    } else if (topR == 6){
        if(leftC == 0){
            sub = 6;
        } else if(leftC == 3){
            sub = 7;
        } else if(leftC == 6){
            sub = 8;
        }   
    }
    if(count != NUMBER_OF_CHILD){
        subgrids[sub] = FALSE;
        printf("%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d invalid!\n", (unsigned
            long)self, topR, bottomR, leftC, rightC);
    }
    else if(count == NUMBER_OF_CHILD){
        subgrids[sub] = TRUE;
        printf("%lx TRow: %d, BRow: %d, LCol: %d, RCol: %d valid!\n", (unsigned
            long)self, topR, bottomR, leftC, rightC);
    }
    pthread_exit(0);
}