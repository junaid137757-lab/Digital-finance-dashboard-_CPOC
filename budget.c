#include <stdio.h>
#include <string.h>

#include "budget.h"
#include "common.h"
#include "persistence.h"
#include "notification.h"
#include "utility.h"

float getSpentForCategory(const char *category)
{
    float spent = 0;
    int i;

    for(i = 0; i < transactionCount; i++)
    {
        if(strcmp(transactions[i].type, "Expense") == 0 &&
           strcmp(transactions[i].category, category) == 0)
        {
            spent += transactions[i].amount;
        }
    }

    return spent;
}

void setBudget()
{
    char category[30];
    float limit;
    int i, found = -1;

    if(budgetCount >= MAX_BUDGETS)
    {
        printf("Budget Limit Reached!\n");
        return;
    }

    printf("Enter Category (or 0 to cancel): ");
    readLine(category, sizeof(category));

    if(strcmp(category, "0") == 0)
    {
        printf("Set Budget Cancelled.\n");
        return;
    }

    printf("Enter Monthly Budget Limit (or -1 to cancel): ");

    if(!readValidFloat(&limit))
        return;

    if(limit == -1)
    {
        printf("Set Budget Cancelled.\n");
        return;
    }

    for(i = 0; i < budgetCount; i++)
    {
        if(strcmp(budgets[i].category, category) == 0)
        {
            found = i;
            break;
        }
    }

    if(found != -1)
    {
        budgets[found].limit = limit;
        printf("Budget Updated Successfully!\n");
    }
    else
    {
        strcpy(budgets[budgetCount].category, category);
        budgets[budgetCount].limit = limit;
        budgetCount++;
        printf("Budget Set Successfully!\n");
    }

    saveBudgets();
}

void viewBudgets()
{
    int i;

    if(budgetCount == 0)
    {
        printf("No Budgets Set\n");
        return;
    }

    printf("\n--------------------------------------------------------------------------------\n");
    printf("%-16s%-12s%-12s%-14s%s\n", "CATEGORY", "LIMIT", "SPENT", "REMAINING", "STATUS");
    printf("--------------------------------------------------------------------------------\n");

    for(i = 0; i < budgetCount; i++)
    {
        float spent = getSpentForCategory(budgets[i].category);
        float remaining = budgets[i].limit - spent;

        printf("%-16s%-12.2f%-12.2f%-14.2f",
               budgets[i].category,
               budgets[i].limit,
               spent,
               remaining);

        if(spent > budgets[i].limit)
            printf("[OVER BUDGET]");
        else if(budgets[i].limit > 0 && spent >= 0.9f * budgets[i].limit)
            printf("[NEAR LIMIT]");
        else
            printf("[OK]");

        printf("\n");
    }
}

void checkBudgetAlert(const char *category)
{
    int i;

    for(i = 0; i < budgetCount; i++)
    {
        if(strcmp(budgets[i].category, category) == 0)
        {
            float spent = getSpentForCategory(category);

            notifyBudgetStatus(category, spent, budgets[i].limit);

            return;
        }
    }
}
