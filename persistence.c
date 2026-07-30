#include <stdio.h>

#include "persistence.h"
#include "common.h"

void loadTransactions()
{
    char filename[50];

    sprintf(filename, "%s_transactions.dat", currentUser);

    FILE *fp = fopen(filename, "rb");

    transactionCount = 0;

    if(fp == NULL)
        return;

    if(fread(&transactionCount, sizeof(int), 1, fp) != 1)
    {
        fclose(fp);
        transactionCount = 0;
        return;
    }

    fread(transactions, sizeof(Transaction), transactionCount, fp);

    fclose(fp);
}

void saveTransactions()
{
    char filename[50];

    sprintf(filename, "%s_transactions.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&transactionCount, sizeof(int), 1, fp);
    fwrite(transactions, sizeof(Transaction), transactionCount, fp);

    fclose(fp);
}

void loadBudgets()
{
    char filename[50];

    sprintf(filename, "%s_budgets.dat", currentUser);

    FILE *fp = fopen(filename, "rb");

    budgetCount = 0;

    if(fp == NULL)
        return;

    if(fread(&budgetCount, sizeof(int), 1, fp) != 1)
    {
        fclose(fp);
        budgetCount = 0;
        return;
    }

    fread(budgets, sizeof(Budget), budgetCount, fp);

    fclose(fp);
}

void saveBudgets()
{
    char filename[50];

    sprintf(filename, "%s_budgets.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&budgetCount, sizeof(int), 1, fp);
    fwrite(budgets, sizeof(Budget), budgetCount, fp);

    fclose(fp);
}

void loadGoals()
{
    char filename[50];

    sprintf(filename, "%s_goals.dat", currentUser);

    FILE *fp = fopen(filename, "rb");

    goalCount = 0;

    if(fp == NULL)
        return;

    if(fread(&goalCount, sizeof(int), 1, fp) != 1)
    {
        fclose(fp);
        goalCount = 0;
        return;
    }

    fread(goals, sizeof(SavingsGoal), goalCount, fp);

    fclose(fp);
}

void saveGoals()
{
    char filename[50];

    sprintf(filename, "%s_goals.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&goalCount, sizeof(int), 1, fp);
    fwrite(goals, sizeof(SavingsGoal), goalCount, fp);

    fclose(fp);
}

void loadEmergencyFund()
{
    char filename[50];

    sprintf(filename, "%s_emergencyfund.dat", currentUser);

    FILE *fp = fopen(filename, "rb");

    emergencyFundBalance = 0;

    if(fp == NULL)
        return;

    fread(&emergencyFundBalance, sizeof(float), 1, fp);

    fclose(fp);
}

void saveEmergencyFund()
{
    char filename[50];

    sprintf(filename, "%s_emergencyfund.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&emergencyFundBalance, sizeof(float), 1, fp);

    fclose(fp);
}
