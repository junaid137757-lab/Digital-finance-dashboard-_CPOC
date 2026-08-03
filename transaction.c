#include <stdio.h>
#include <string.h>

#include "transaction.h"
#include "common.h"
#include "persistence.h"
#include "budget.h"
#include "utility.h"

void viewTransactions()
{
    int i;
    float balance = 0;

    if(transactionCount == 0)
    {
        printf("No Transactions Available\n");
        return;
    }

    printf("\n--------------------------------------------------------------------------------\n");
    printf("%-5s%-10s%-16s%-12s%-14s%-12s\n", "ID", "TYPE", "CATEGORY", "AMOUNT", "DATE", "BALANCE");
    printf("--------------------------------------------------------------------------------\n");

    for(i = 0; i < transactionCount; i++)
    {
        if(strcmp(transactions[i].type, "Income") == 0)
            balance += transactions[i].amount;
        else
            balance -= transactions[i].amount;

        printf("%-5d%-10s%-16s%-12.2f%-14s%-12.2f\n",
               transactions[i].id,
               transactions[i].type,
               transactions[i].category,
               transactions[i].amount,
               transactions[i].date,
               balance);
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("Running Balance (Total Income - Total Expense): %.2f\n", balance);
}

void editTransaction()
{
    int id, i, found = -1;
    char newCategory[30] = "";
    float newAmount;

    viewTransactions();

    if(transactionCount == 0)
        return;

    printf("\nEnter ID of Transaction to Edit (or 0 to cancel): ");

    if(!readValidInt(&id))
        return;

    if(id == 0)
    {
        printf("Edit Cancelled.\n");
        return;
    }

    for(i = 0; i < transactionCount; i++)
    {
        if(transactions[i].id == id)
        {
            found = i;
            break;
        }
    }

    if(found == -1)
    {
        printf("Transaction Not Found!\n");
        return;
    }

    printf("Enter New Category (or 0 to cancel): ");
    readLine(newCategory, sizeof(newCategory));

    if(strcmp(newCategory, "0") == 0)
    {
        printf("Edit Cancelled.\n");
        return;
    }

    printf("Enter New Amount (or -1 to cancel): ");

    if(!readValidFloat(&newAmount))
        return;

    if(newAmount == -1)
    {
        printf("Edit Cancelled.\n");
        return;
    }

    strcpy(transactions[found].category, newCategory);
    transactions[found].amount = newAmount;

    saveTransactions();

    printf("Transaction Updated Successfully!\n");

    if(strcmp(transactions[found].type, "Expense") == 0)
        checkBudgetAlert(transactions[found].category);
}

void deleteTransaction()
{
    int id, i, found = -1;

    viewTransactions();

    if(transactionCount == 0)
        return;

    printf("\nEnter ID of Transaction to Delete (or 0 to cancel): ");

    if(!readValidInt(&id))
        return;

    if(id == 0)
    {
        printf("Delete Cancelled.\n");
        return;
    }

    for(i = 0; i < transactionCount; i++)
    {
        if(transactions[i].id == id)
        {
            found = i;
            break;
        }
    }

    if(found == -1)
    {
        printf("Transaction Not Found!\n");
        return;
    }

    for(i = found; i < transactionCount - 1; i++)
        transactions[i] = transactions[i + 1];

    transactionCount--;

    saveTransactions();

    printf("Transaction Deleted Successfully!\n");
}

static int printMatchingTransactions(const char *category)
{
    int i, found = 0;

    for(i = 0; i < transactionCount; i++)
    {
        if(strcmp(transactions[i].category, category) == 0)
        {
            if(!found)
            {
                printf("\n--------------------------------------------------------------------\n");
                printf("%-5s%-10s%-16s%-12s%-14s\n", "ID", "TYPE", "CATEGORY", "AMOUNT", "DATE");
                printf("--------------------------------------------------------------------\n");
            }

            printf("%-5d%-10s%-16s%-12.2f%-14s\n",
                   transactions[i].id,
                   transactions[i].type,
                   transactions[i].category,
                   transactions[i].amount,
                   transactions[i].date);

            found = 1;
        }
    }

    return found;
}

void searchTransactionsByCategory()
{
    char category[30] = "";
    char suggestions[10][30];
    int suggestionCount = 0;
    int i, j;
    int suggestionChoice;

    printf("Enter Category to Search (or 0 to cancel): ");
    readLine(category, sizeof(category));

    if(strcmp(category, "0") == 0)
    {
        printf("Search Cancelled.\n");
        return;
    }

    if(printMatchingTransactions(category))
        return;

    printf("No Transactions Found In This Category\n");

    /* No exact match - offer "Did you mean" suggestions based on a
       case-insensitive partial match, the way a search engine would,
       instead of just giving up (e.g. searching "income" when the
       actual category is "stocks income"). */
    for(i = 0; i < transactionCount && suggestionCount < 10; i++)
    {
        int alreadyListed = 0;

        for(j = 0; j < suggestionCount; j++)
        {
            if(strcmp(suggestions[j], transactions[i].category) == 0)
            {
                alreadyListed = 1;
                break;
            }
        }

        if(!alreadyListed &&
           (caseInsensitiveContains(transactions[i].category, category) ||
            caseInsensitiveContains(category, transactions[i].category)))
        {
            strcpy(suggestions[suggestionCount], transactions[i].category);
            suggestionCount++;
        }
    }

    if(suggestionCount == 0)
        return;

    printf("\nDid you mean:\n");

    for(i = 0; i < suggestionCount; i++)
        printf("  %d. %s\n", i + 1, suggestions[i]);

    printf("Enter number to search that category (or 0 to skip): ");

    if(!readValidInt(&suggestionChoice))
        return;

    if(suggestionChoice < 1 || suggestionChoice > suggestionCount)
        return;

    printMatchingTransactions(suggestions[suggestionChoice - 1]);
}
