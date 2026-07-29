# Testing Summary - Digital Personal Finance Platform

## 1. Build Verification

Compiled with strict warnings enabled, zero warnings/errors:

```
gcc src/main.c src/globals.c src/utility.c src/notification.c \
    src/authentication.c src/persistence.c src/income.c src/expense.c \
    src/transaction.c src/budget.c src/savings_goal.c src/dashboard.c \
    src/reports.c src/recommendation.c \
    -Iinclude -o financeapp -Wall -Wextra
```

Result: clean build, no warnings.

## 2. Static Analysis

Tool: `cppcheck`

```
cppcheck --enable=warning,style src/
```

All flagged `uninitvar` false positives (readLine()-filled buffers not
visible cross-file to cppcheck) resolved by initializing buffers at
declaration. Re-run and confirm no remaining warnings.

## 3. Automated Functional Testing

Tool: `test.sh` (included) - drives the compiled binary through scripted
input sequences and checks the output against expected results.

```
chmod +x test.sh
./test.sh
```

### Coverage (26 automated cases)

| Area | Cases |
|---|---|
| User Management | Register, duplicate rejection, weak password rejection, cancel, login success/failure/cancel, forgot password, change password (correct/incorrect), multi-word username distinctness |
| Income / Expense | Add income, cancel via category, invalid amount rejected, multi-word category |
| Transaction Management | Invalid ID rejected, edit success, delete success, delete cancel, search by multi-word category |
| Budget Planning & Tracking | Over-budget alert, View Budgets status flag |
| Savings Goal Management | Goal funded -> congratulations message |
| Financial Dashboard | Correct income total |
| Smart Recommendations | Missing emergency fund flagged |
| Data Isolation | New user's data file starts empty (no cross-user leakage) |

Latest run: **26/26 passed**.

## 4. Manual / Exploratory Testing (recommended before sign-off)

Automated tests cover input/output correctness but not the interactive
feel. Manually walk through each of the 11 features once end-to-end in
a real terminal session, checking:

- Menu navigation and formatting look correct
- Prompts display in the right order (no buffering artifacts)
- Reports/dashboard numbers match manually-calculated expectations
- Behavior with a large number of transactions (bulk add, then view/report)

## 5. Known Limitations (documented, not blocking for POC)

- Passwords stored in plaintext in `users.dat` (acceptable for POC scope;
  flagged for future hashing if this moves beyond POC)
- Single-machine, file-based storage - no concurrent multi-session support
- No automated unit tests at the function level, only end-to-end CLI tests

## 6. Sign-off Checklist

- [ ] Clean compile with `-Wall -Wextra`
- [ ] `cppcheck` run with no unresolved warnings
- [ ] `./test.sh` passes 100%
- [ ] Manual walkthrough of all 11 features completed
- [ ] `.dat` files and compiled binaries excluded from final package
