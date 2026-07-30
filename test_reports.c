#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "reports.h"
#include "common.h"

static int suite_init(void)
{
    th_reset_globals();
    return 0;
}

static int suite_clean(void)
{
    return 0;
}

static void add_transaction(const char *type, const char *category,
                             float amount, const char *date)
{
    Transaction *t = &transactions[transactionCount];

    strcpy(t->type, type);
    strcpy(t->category, category);
    t->amount = amount;
    strcpy(t->date, date);
    t->id = ++transactionCount;
}

/* With no transactions, both reports should say so. */
static void test_categoryWiseReport_empty(void)
{
    th_reset_globals();

    th_capture_stdout_start();
    categoryWiseReport();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "No Transactions Available") != NULL);
    free(out);
}

/* Net amount per category should be Expense-positive,
   Income-negative, summed correctly, per the report's own
   documented convention. */
static void test_categoryWiseReport_nets_by_category(void)
{
    th_reset_globals();

    add_transaction("Expense", "Food", 100.0f, "2026-01-01");
    add_transaction("Expense", "Food", 50.0f, "2026-01-05");
    add_transaction("Income", "Food", 30.0f, "2026-01-10"); /* e.g. a refund */

    th_capture_stdout_start();
    categoryWiseReport();
    char *out = th_capture_stdout_end();

    /* Net for Food = 100 + 50 - 30 = 120.00 */
    CU_ASSERT(strstr(out, "Food") != NULL);
    CU_ASSERT(strstr(out, "120.00") != NULL);
    free(out);
}

/* With no transactions, monthlyReport() should say so too. */
static void test_monthlyReport_empty(void)
{
    th_reset_globals();

    th_capture_stdout_start();
    monthlyReport();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "No Transactions Available") != NULL);
    free(out);
}

/* Transactions should be grouped by the YYYY-MM prefix of their
   date, with income/expense/net computed per month. */
static void test_monthlyReport_groups_by_month(void)
{
    th_reset_globals();

    add_transaction("Income", "Salary", 3000.0f, "2026-01-01");
    add_transaction("Expense", "Rent", 1000.0f, "2026-01-05");
    add_transaction("Income", "Salary", 3000.0f, "2026-02-01");

    th_capture_stdout_start();
    monthlyReport();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "2026-01") != NULL);
    CU_ASSERT(strstr(out, "2026-02") != NULL);
    CU_ASSERT(strstr(out, "2000.00") != NULL); /* Jan net: 3000 - 1000 */
    free(out);
}

void add_reports_suite(void)
{
    CU_pSuite suite = CU_add_suite("reports", suite_init, suite_clean);

    CU_add_test(suite, "categoryWiseReport reports empty ledger", test_categoryWiseReport_empty);
    CU_add_test(suite, "categoryWiseReport nets amounts by category", test_categoryWiseReport_nets_by_category);
    CU_add_test(suite, "monthlyReport reports empty ledger", test_monthlyReport_empty);
    CU_add_test(suite, "monthlyReport groups transactions by month", test_monthlyReport_groups_by_month);
}
