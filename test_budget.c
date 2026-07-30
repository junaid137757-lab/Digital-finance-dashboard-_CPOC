#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "budget.h"
#include "common.h"

/* setBudget()/viewBudgets() call saveBudgets(), which writes
   "<currentUser>_budgets.dat" into the working directory - so this
   suite runs from a scratch temp dir. */
static int suite_init(void)
{
    th_enter_tmp_dir();
    th_reset_globals();
    strcpy(currentUser, "buduser");
    return 0;
}

static int suite_clean(void)
{
    th_leave_tmp_dir();
    return 0;
}

static void add_transaction(const char *type, const char *category, float amount)
{
    Transaction *t = &transactions[transactionCount];

    strcpy(t->type, type);
    strcpy(t->category, category);
    t->amount = amount;
    strcpy(t->date, "2026-01-01");
    t->id = ++transactionCount;
}

/* getSpentForCategory() should sum only Expense transactions in
   the requested category, ignoring Income and other categories. */
static void test_getSpentForCategory_sums_expenses_only(void)
{
    th_reset_globals();

    add_transaction("Expense", "Food", 100.0f);
    add_transaction("Expense", "Food", 50.0f);
    add_transaction("Income", "Food", 500.0f);   /* must be ignored */
    add_transaction("Expense", "Travel", 75.0f); /* different category */

    float spent = getSpentForCategory("Food");

    CU_ASSERT_DOUBLE_EQUAL(spent, 150.0, 0.001);
}

/* A category with no matching transactions should sum to 0. */
static void test_getSpentForCategory_no_match_is_zero(void)
{
    th_reset_globals();

    add_transaction("Expense", "Food", 100.0f);

    float spent = getSpentForCategory("Rent");

    CU_ASSERT_DOUBLE_EQUAL(spent, 0.0, 0.001);
}

/* setBudget() with a brand-new category should add a budget entry
   and report success. */
static void test_setBudget_new_category(void)
{
    th_reset_globals();

    th_feed_stdin("Food\n300\n");
    th_capture_stdout_start();
    setBudget();
    char *out = th_capture_stdout_end();

    CU_ASSERT_EQUAL(budgetCount, 1);
    CU_ASSERT_STRING_EQUAL(budgets[0].category, "Food");
    CU_ASSERT_DOUBLE_EQUAL(budgets[0].limit, 300.0, 0.001);
    CU_ASSERT(strstr(out, "Budget Set Successfully") != NULL);
    free(out);
}

/* setBudget() on a category that already has a budget should
   update the existing entry in place rather than adding a
   duplicate one. */
static void test_setBudget_existing_category_updates(void)
{
    th_reset_globals();
    strcpy(budgets[0].category, "Food");
    budgets[0].limit = 300.0f;
    budgetCount = 1;

    th_feed_stdin("Food\n450\n");
    th_capture_stdout_start();
    setBudget();
    char *out = th_capture_stdout_end();

    CU_ASSERT_EQUAL(budgetCount, 1);
    CU_ASSERT_DOUBLE_EQUAL(budgets[0].limit, 450.0, 0.001);
    CU_ASSERT(strstr(out, "Budget Updated Successfully") != NULL);
    free(out);
}

/* Entering "0" for the category should cancel without touching
   budgetCount. */
static void test_setBudget_cancel_via_category(void)
{
    th_reset_globals();

    th_feed_stdin("0\n");
    th_capture_stdout_start();
    setBudget();
    char *out = th_capture_stdout_end();

    CU_ASSERT_EQUAL(budgetCount, 0);
    CU_ASSERT(strstr(out, "Cancelled") != NULL);
    free(out);
}

/* Entering -1 for the limit should cancel after the category has
   already been entered, without adding a budget. */
static void test_setBudget_cancel_via_limit(void)
{
    th_reset_globals();

    th_feed_stdin("Food\n-1\n");
    th_capture_stdout_start();
    setBudget();
    char *out = th_capture_stdout_end();

    CU_ASSERT_EQUAL(budgetCount, 0);
    CU_ASSERT(strstr(out, "Cancelled") != NULL);
    free(out);
}

/* When MAX_BUDGETS is already reached, setBudget() must refuse to
   add another one and must not read/consume any input. */
static void test_setBudget_limit_reached(void)
{
    th_reset_globals();
    budgetCount = MAX_BUDGETS;

    th_capture_stdout_start();
    setBudget();
    char *out = th_capture_stdout_end();

    CU_ASSERT_EQUAL(budgetCount, MAX_BUDGETS);
    CU_ASSERT(strstr(out, "Budget Limit Reached") != NULL);
    free(out);
}

/* viewBudgets() with no budgets set should say so and not print
   a table. */
static void test_viewBudgets_empty(void)
{
    th_reset_globals();

    th_capture_stdout_start();
    viewBudgets();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "No Budgets Set") != NULL);
    free(out);
}

/* viewBudgets() should flag a category that's over its limit as
   [OVER BUDGET] and one safely under it as [OK]. */
static void test_viewBudgets_flags_over_and_ok(void)
{
    th_reset_globals();

    strcpy(budgets[0].category, "Food");
    budgets[0].limit = 100.0f;
    strcpy(budgets[1].category, "Travel");
    budgets[1].limit = 500.0f;
    budgetCount = 2;

    add_transaction("Expense", "Food", 150.0f);   /* over */
    add_transaction("Expense", "Travel", 50.0f);  /* well under */

    th_capture_stdout_start();
    viewBudgets();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "[OVER BUDGET]") != NULL);
    CU_ASSERT(strstr(out, "[OK]") != NULL);
    free(out);
}

/* checkBudgetAlert() should look up the budget for the given
   category and forward spent/limit into the notification layer,
   producing an [ALERT] when over budget. */
static void test_checkBudgetAlert_triggers_alert_when_over(void)
{
    th_reset_globals();

    strcpy(budgets[0].category, "Food");
    budgets[0].limit = 100.0f;
    budgetCount = 1;

    add_transaction("Expense", "Food", 120.0f);

    th_capture_stdout_start();
    checkBudgetAlert("Food");
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "[ALERT]") != NULL);
    free(out);
}

/* checkBudgetAlert() for a category with no budget set at all
   should do nothing (no crash, no output). */
static void test_checkBudgetAlert_no_budget_for_category(void)
{
    th_reset_globals();

    th_capture_stdout_start();
    checkBudgetAlert("Untracked");
    char *out = th_capture_stdout_end();

    CU_ASSERT_STRING_EQUAL(out, "");
    free(out);
}

void add_budget_suite(void)
{
    CU_pSuite suite = CU_add_suite("budget", suite_init, suite_clean);

    CU_add_test(suite, "getSpentForCategory sums expenses only", test_getSpentForCategory_sums_expenses_only);
    CU_add_test(suite, "getSpentForCategory is 0 with no match", test_getSpentForCategory_no_match_is_zero);
    CU_add_test(suite, "setBudget adds a new category", test_setBudget_new_category);
    CU_add_test(suite, "setBudget updates an existing category", test_setBudget_existing_category_updates);
    CU_add_test(suite, "setBudget cancels via category '0'", test_setBudget_cancel_via_category);
    CU_add_test(suite, "setBudget cancels via limit -1", test_setBudget_cancel_via_limit);
    CU_add_test(suite, "setBudget refuses past MAX_BUDGETS", test_setBudget_limit_reached);
    CU_add_test(suite, "viewBudgets reports empty state", test_viewBudgets_empty);
    CU_add_test(suite, "viewBudgets flags OVER BUDGET and OK", test_viewBudgets_flags_over_and_ok);
    CU_add_test(suite, "checkBudgetAlert alerts when over budget", test_checkBudgetAlert_triggers_alert_when_over);
    CU_add_test(suite, "checkBudgetAlert is silent with no budget", test_checkBudgetAlert_no_budget_for_category);
}
