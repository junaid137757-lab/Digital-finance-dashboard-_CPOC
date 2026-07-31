#include <stdio.h>

#include "persistence.h"
#include "common.h"

void loadTransactions(void)
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

    fread(transactions, sizeof(Transaction), (size_t)transactionCount, fp);

    fclose(fp);
}

void saveTransactions(void)
{
    char filename[50];

    sprintf(filename, "%s_transactions.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&transactionCount, sizeof(int), 1, fp);
    fwrite(transactions, sizeof(Transaction), (size_t)transactionCount, fp);

    fclose(fp);
}

void loadBudgets(void)
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

    fread(budgets, sizeof(Budget), (size_t)budgetCount, fp);

    fclose(fp);
}

void saveBudgets(void)
{
    char filename[50];

    sprintf(filename, "%s_budgets.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&budgetCount, sizeof(int), 1, fp);
    fwrite(budgets, sizeof(Budget), (size_t)budgetCount, fp);

    fclose(fp);
}

void loadGoals(void)
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

    fread(goals, sizeof(SavingsGoal), (size_t)goalCount, fp);

    fclose(fp);
}

void saveGoals(void)
{
    char filename[50];

    sprintf(filename, "%s_goals.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&goalCount, sizeof(int), 1, fp);
    fwrite(goals, sizeof(SavingsGoal), (size_t)goalCount, fp);

    fclose(fp);
}

void loadEmergencyFund(void)
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

void saveEmergencyFund(void)
{
    char filename[50];

    sprintf(filename, "%s_emergencyfund.dat", currentUser);

    FILE *fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fwrite(&emergencyFundBalance, sizeof(float), 1, fp);

    fclose(fp);
}
