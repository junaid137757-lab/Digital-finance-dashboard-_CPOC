#ifndef BUDGET_H
#define BUDGET_H

void setBudget(void);
void viewBudgets(void);
void checkBudgetAlert(const char *category);

/* Exposed so Dashboard and Recommendation modules can
   read live spend-per-category without duplicating logic. */
float getSpentForCategory(const char *category);

#endif
