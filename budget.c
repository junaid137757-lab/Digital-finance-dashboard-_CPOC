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
    char category[30] = "";
    char existingCategories[50][30];
    int existingCount = 0;
    float limit;
    int i, j, found = -1;
    int choice;

    if(budgetCount >= MAX_BUDGETS)
    {
        printf("Budget Limit Reached!\n");
        return;
    }

    /* Only let the user set a budget for a category that's actually
       been used in a transaction, rather than free-typing a name
       that might not exist or have a typo - show the real
       categories first, then require picking one of them. */
    for(i = 0; i < transactionCount && existingCount < 50; i++)
    {
        int alreadyListed = 0;

        for(j = 0; j < existingCount; j++)
        {
            if(strcmp(existingCategories[j], transactions[i].category) == 0)
            {
                alreadyListed = 1;
                break;
            }
        }

        if(!alreadyListed)
        {
            strcpy(existingCategories[existingCount], transactions[i].category);
            existingCount++;
        }
    }

    if(existingCount == 0)
    {
        printf("No transaction categories available yet. Add an income or\n");
        printf("expense first, then set a budget for that category.\n");
        return;
    }

    printf("\nExisting Categories:\n");

    for(i = 0; i < existingCount; i++)
        printf("  %d. %s\n", i + 1, existingCategories[i]);

    printf("Select a Category by Number (or 0 to cancel): ");

    if(!readValidInt(&choice))
        return;

    if(choice == 0)
    {
        printf("Set Budget Cancelled.\n");
        return;
    }

    if(choice < 1 || choice > existingCount)
    {
        printf("Invalid Choice\n");
        return;
    }

    strcpy(category, existingCategories[choice - 1]);

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
