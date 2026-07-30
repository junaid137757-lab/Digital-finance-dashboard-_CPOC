#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <string.h>

#include "test_helpers.h"
#include "transaction.h"
#include "common.h"

/* editTransaction()/deleteTransaction() call saveTransactions(). */
static int suite_init(void)
{
    th_enter_tmp_dir();
    th_reset_globals();
    strcpy(currentUser, "txnuser");
    return 0;
}

static int suite_clean(void)
{
    th_leave_tmp_dir();
    return 0;
}

static void seed_two_transactions(void)
{
    transactions[0].id = 1;
    strcpy(transactions[0].type, "Income");
    strcpy(transactions[0].category, "Salary");
    transactions[0].amount = 3000.0f;
    strcpy(transactions[0].date, "2026-01-01");

    transactions[1].id = 2;
    strcpy(transactions[1].type, "Expense");
    strcpy(transactions[1].category, "Rent");
    transactions[1].amount = 1000.0f;
    strcpy(transactions[1].date, "2026-01-02");

    transactionCount = 2;
}

/* With no transactions, viewTransactions() should say so and
   print no table. */
static void test_viewTransactions_empty(void)
{
    th_reset_globals();

    th_capture_stdout_start();
    viewTransactions();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "No Transactions Available") != NULL);
    free(out);
}

/* The running balance should be Income-minus-Expense, computed
   correctly across the ledger. */
static void test_viewTransactions_computes_balance(void)
{
    th_reset_globals();
    seed_two_transactions();

    th_capture_stdout_start();
    viewTransactions();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "2000.00") != NULL); /* 3000 income - 1000 expense */
    free(out);
}

/* editTransaction() on an existing id should update its category
   and amount and report success. */
static void test_editTransaction_success(void)
{
    th_reset_globals();
    strcpy(currentUser, "txnuser");
    seed_two_transactions();

    th_feed_stdin("2\nUtilities\n1100\n");
    th_capture_stdout_start();
    editTransaction();
    char *out = th_capture_stdout_end();

    CU_ASSERT_STRING_EQUAL(transactions[1].category, "Utilities");
    CU_ASSERT_DOUBLE_EQUAL(transactions[1].amount, 1100.0, 0.001);
    CU_ASSERT(strstr(out, "Transaction Updated Successfully") != NULL);
    free(out);
}

/* Editing an id that doesn't exist should report "not found" and
   leave the data untouched. */
static void test_editTransaction_id_not_found(void)
{
    th_reset_globals();
    strcpy(currentUser, "txnuser");
    seed_two_transactions();

    th_feed_stdin("999\n");
    th_capture_stdout_start();
    editTransaction();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "Transaction Not Found") != NULL);
    CU_ASSERT_STRING_EQUAL(transactions[1].category, "Rent"); /* unchanged */
    free(out);
}

/* Entering "0" for the id should cancel the edit. */
static void test_editTransaction_cancel(void)
{
    th_reset_globals();
    strcpy(currentUser, "txnuser");
    seed_two_transactions();

    th_feed_stdin("0\n");
    th_capture_stdout_start();
    editTransaction();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "Edit Cancelled") != NULL);
    free(out);
}

/* deleteTransaction() should remove the matching entry, shift the
   remaining ones down, and decrement the count. */
static void test_deleteTransaction_success(void)
{
    th_reset_globals();
    strcpy(currentUser, "txnuser");
    seed_two_transactions();

    th_feed_stdin("1\n");
    th_capture_stdout_start();
    deleteTransaction();
    char *out = th_capture_stdout_end();

    CU_ASSERT_EQUAL(transactionCount, 1);
    CU_ASSERT_EQUAL(transactions[0].id, 2);
    CU_ASSERT_STRING_EQUAL(transactions[0].category, "Rent");
    CU_ASSERT(strstr(out, "Transaction Deleted Successfully") != NULL);
    free(out);
}

/* Entering "0" for the id should cancel the delete, leaving the
   ledger untouched. */
static void test_deleteTransaction_cancel(void)
{
    th_reset_globals();
    strcpy(currentUser, "txnuser");
    seed_two_transactions();

    th_feed_stdin("0\n");
    th_capture_stdout_start();
    deleteTransaction();
    char *out = th_capture_stdout_end();

    CU_ASSERT_EQUAL(transactionCount, 2);
    CU_ASSERT(strstr(out, "Delete Cancelled") != NULL);
    free(out);
}

/* Searching for a category that exists should list only matching
   rows. */
static void test_search_finds_matching_category(void)
{
    th_reset_globals();
    seed_two_transactions();

    th_feed_stdin("Rent\n");
    th_capture_stdout_start();
    searchTransactionsByCategory();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "Rent") != NULL);
    CU_ASSERT(strstr(out, "Salary") == NULL);
    free(out);
}

/* Searching for a category with no matches should say so. */
static void test_search_no_match(void)
{
    th_reset_globals();
    seed_two_transactions();

    th_feed_stdin("Travel\n");
    th_capture_stdout_start();
    searchTransactionsByCategory();
    char *out = th_capture_stdout_end();

    CU_ASSERT(strstr(out, "No Transactions Found In This Category") != NULL);
    free(out);
}

void add_transaction_suite(void)
{
    CU_pSuite suite = CU_add_suite("transaction", suite_init, suite_clean);

    CU_add_test(suite, "viewTransactions reports empty ledger", test_viewTransactions_empty);
    CU_add_test(suite, "viewTransactions computes running balance", test_viewTransactions_computes_balance);
    CU_add_test(suite, "editTransaction updates an existing entry", test_editTransaction_success);
    CU_add_test(suite, "editTransaction reports id not found", test_editTransaction_id_not_found);
    CU_add_test(suite, "editTransaction cancels via id '0'", test_editTransaction_cancel);
    CU_add_test(suite, "deleteTransaction removes and shifts entries", test_deleteTransaction_success);
    CU_add_test(suite, "deleteTransaction cancels via id '0'", test_deleteTransaction_cancel);
    CU_add_test(suite, "search finds matching category", test_search_finds_matching_category);
    CU_add_test(suite, "search reports no match", test_search_no_match);
}
