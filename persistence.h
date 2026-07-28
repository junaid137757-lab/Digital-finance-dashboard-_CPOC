#ifndef PERSISTENCE_H
#define PERSISTENCE_H

/* Data Storage & Persistence module: reads/writes the
   three per-user .dat files (transactions, budgets, goals). */

void loadTransactions();
void saveTransactions();

void loadBudgets();
void saveBudgets();

void loadGoals();
void saveGoals();

#endif
