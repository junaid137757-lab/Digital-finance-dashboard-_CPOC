#include <stdio.h>
#include <string.h>

#include "expense.h"
#include "common.h"
#include "utility.h"
#include "persistence.h"
#include "budget.h"

void addExpense()
{
    Transaction t;

    if(transactionCount >= MAX)
    {
        printf("Transaction Limit Reached!\n");
        return;
    }

    strcpy(t.type, "Expense");

    printf("Enter Expense Category (or 0 to cancel): ");
    readLine(t.category, sizeof(t.category));

    if(strcmp(t.category, "0") == 0)
    {
        printf("Add Expense Cancelled.\n");
        return;
    }

    printf("Enter Amount (or -1 to cancel): ");

    if(!readValidFloat(&t.amount))
        return;

    if(t.amount == -1)
    {
        printf("Add Expense Cancelled.\n");
        return;
    }

    t.id = transactionCount + 1;
    getCurrentDate(t.date);

    transactions[transactionCount++] = t;

    saveTransactions();

    printf("Expense Added Successfully!\n");

    checkBudgetAlert(t.category);
}
