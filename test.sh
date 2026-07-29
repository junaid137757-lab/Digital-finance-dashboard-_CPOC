#!/bin/bash
# ============================================================
# test.sh - Automated smoke tests for the Digital Personal
# Finance Platform.
#
# Usage:
#   chmod +x test.sh
#   ./test.sh
#
# Each test runs the compiled binary with a scripted sequence
# of keystrokes (via printf | ./financeapp) and checks that the
# expected text appears somewhere in the output. Data files are
# wiped before each test so tests don't interfere with each
# other.
# ============================================================

BINARY=./financeapp
PASS=0
FAIL=0
FAILED_NAMES=()

run_test() {
    local name="$1"
    local input="$2"
    local expect="$3"

    rm -f *.dat

    local output
    output=$(printf "$input" | timeout 5 "$BINARY" 2>&1)

    if echo "$output" | grep -qF "$expect"; then
        echo "PASS: $name"
        PASS=$((PASS + 1))
    else
        echo "FAIL: $name"
        echo "  --- expected to contain: $expect"
        echo "  --- actual output:"
        echo "$output" | sed 's/^/      /'
        FAIL=$((FAIL + 1))
        FAILED_NAMES+=("$name")
    fi
}

if [ ! -f "$BINARY" ]; then
    echo "Error: $BINARY not found. Build the project first."
    exit 1
fi

echo "============================================"
echo " Running automated smoke tests"
echo "============================================"

# ---------- User Management ----------
run_test "Register new user" \
    "1\nalice\nAlice@123\n4\n" \
    "Account Created Successfully"

run_test "Register rejects duplicate username" \
    "1\nalice\nAlice@123\n1\nalice\nBob@1234\n4\n" \
    "Username Already Exists"

run_test "Register rejects weak password, then accepts strong one" \
    "1\nbob\nweakpass\nBob@1234\n4\n" \
    "Password does not meet the requirements"

run_test "Register can be cancelled" \
    "1\n0\n4\n" \
    "Registration Cancelled"

run_test "Login succeeds with correct credentials" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n17\n4\n" \
    "Login Successful"

run_test "Login rejects wrong password" \
    "1\nalice\nAlice@123\n2\nalice\nWrongPass1!\n4\n" \
    "Invalid Username or Password"

run_test "Login can be cancelled" \
    "1\nalice\nAlice@123\n2\n0\n4\n" \
    "Login Cancelled"

run_test "Forgot Password resets and new password works" \
    "1\nalice\nAlice@123\n3\nalice\nNewPass@12\n2\nalice\nNewPass@12\n17\n4\n" \
    "Login Successful"

run_test "Change Password rejects wrong current password" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n16\nWrongOld1!\n17\n4\n" \
    "Incorrect Password"

run_test "Change Password succeeds with correct current password" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n16\nAlice@123\nNewPass@99\n17\n4\n" \
    "Password Changed Successfully"

run_test "Distinct multi-word username is not treated as duplicate" \
    "1\nsantosh\nSant@1234\n1\nSantosh Kumar\nKuma@1234\n4\n" \
    "Account Created Successfully"

# ---------- Income / Expense ----------
run_test "Add Income works and shows in transaction list" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\n50000\n3\n17\n4\n" \
    "salary"

run_test "Add Income can be cancelled via category" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\n0\n17\n4\n" \
    "Add Income Cancelled"

run_test "Add Income rejects non-numeric amount" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\nabc\n17\n4\n" \
    "Invalid input - please enter a number"

run_test "Add Expense with multi-word category saves correctly" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n2\nmedical bills\n500\n3\n17\n4\n" \
    "medical bills"

# ---------- Transaction Management ----------
run_test "Edit Transaction rejects non-numeric ID" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\n50000\n4\nhouse rent\n17\n4\n" \
    "Invalid input - please enter a whole number"

run_test "Edit Transaction updates successfully with valid ID" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\n50000\n4\n1\nbonus\n60000\n17\n4\n" \
    "Transaction Updated Successfully"

run_test "Delete Transaction works with valid ID" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\n50000\n5\n1\n17\n4\n" \
    "Transaction Deleted Successfully"

run_test "Delete Transaction can be cancelled" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\n50000\n5\n0\n17\n4\n" \
    "Delete Cancelled"

run_test "Search Transactions finds multi-word category" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n2\nmedical bills\n500\n6\nmedical bills\n17\n4\n" \
    "medical bills"

# ---------- Budget Planning & Tracking ----------
run_test "Budget alert fires when expense exceeds limit" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n7\nfood\n1000\n2\nfood\n1500\n17\n4\n" \
    "exceeded your budget"

run_test "View Budgets flags an over-limit category" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n7\nfood\n1000\n2\nfood\n1500\n8\n17\n4\n" \
    "OVER BUDGET"

# ---------- Savings Goal Management ----------
run_test "Savings Goal reaches CONGRATULATIONS message when funded" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n9\nEmergency Fund\n1000\n10\nEmergency Fund\n1000\n17\n4\n" \
    "CONGRATULATIONS"

# ---------- Financial Dashboard ----------
run_test "Dashboard shows correct total income" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\n1000\n12\n17\n4\n" \
    "Total Income     : 1000.00"

# ---------- Smart Financial Recommendation Engine ----------
run_test "Recommendations flag missing emergency fund" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalary\n1000\n15\n17\n4\n" \
    "emergency fund"

# ---------- Multi-user data isolation ----------
run_test "New user's transaction file starts empty (no cross-user leakage)" \
    "1\nalice\nAlice@123\n2\nalice\nAlice@123\n1\nsalaryonly\n1000\n17\n1\nbob\nBob@12345\n2\nbob\nBob@12345\n3\n17\n4\n" \
    "No Transactions Available"

echo "============================================"
echo " Results: $PASS passed, $FAIL failed"
echo "============================================"

if [ $FAIL -gt 0 ]; then
    echo "Failed tests:"
    for n in "${FAILED_NAMES[@]}"; do
        echo "  - $n"
    done
    exit 1
fi

exit 0
