#ifndef COMMON_H
#define COMMON_H

/* =====================================================
   Shared structs, limits, and global data used by
   every module. Each module includes this to access
   the data it needs, but owns none of it directly -
   the actual storage lives in globals.c
   ===================================================== */

#define MAX 1000
#define MAX_BUDGETS 100
#define MAX_GOALS 50

typedef struct
{
    char username[30];
    char password[30];
} User;

typedef struct
{
    int id;
    char type[20];
    char category[30];
    float amount;
    char date[11];
} Transaction;

typedef struct
{
    char category[30];
    float limit;
} Budget;

typedef struct
{
    char name[50];
    float targetAmount;
    float savedAmount;
} SavingsGoal;

extern Transaction transactions[MAX];
extern int transactionCount;

extern Budget budgets[MAX_BUDGETS];
extern int budgetCount;

extern SavingsGoal goals[MAX_GOALS];
extern int goalCount;

extern char currentUser[30];

#endif
