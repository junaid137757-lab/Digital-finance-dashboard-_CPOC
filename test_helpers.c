#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/stat.h>

#include "test_helpers.h"
#include "common.h"

/* ---- global state reset -------------------------------------- */

void th_reset_globals(void)
{
    transactionCount = 0;
    memset(transactions, 0, sizeof(transactions));

    budgetCount = 0;
    memset(budgets, 0, sizeof(budgets));

    goalCount = 0;
    memset(goals, 0, sizeof(goals));

    memset(currentUser, 0, sizeof(currentUser));
}

/* ---- stdin injection ------------------------------------------ */

static FILE *th_stdin_stream = NULL;
static char *th_stdin_buffer = NULL;

void th_feed_stdin(const char *input)
{
    if(th_stdin_stream != NULL)
    {
        fclose(th_stdin_stream);
        th_stdin_stream = NULL;
    }

    free(th_stdin_buffer);
    th_stdin_buffer = strdup(input);

    th_stdin_stream = fmemopen(th_stdin_buffer, strlen(th_stdin_buffer), "r");

    stdin = th_stdin_stream;
}

/* ---- stdout capture --------------------------------------------
   NOTE: the app's own code (main.c) sets stdout to unbuffered via
   setvbuf(); that's not linked into the test binary, so this is
   safe. We swap the process-wide `stdout` FILE* itself (a plain
   variable in glibc) rather than dup()'ing fd 1, which keeps this
   simple and self-contained per test. */

static FILE *th_real_stdout = NULL;
static char *th_stdout_buf = NULL;
static size_t th_stdout_size = 0;
static FILE *th_stdout_stream = NULL;

void th_capture_stdout_start(void)
{
    fflush(stdout);

    th_real_stdout = stdout;
    th_stdout_buf = NULL;
    th_stdout_size = 0;

    th_stdout_stream = open_memstream(&th_stdout_buf, &th_stdout_size);
    stdout = th_stdout_stream;
}

char *th_capture_stdout_end(void)
{
    fflush(th_stdout_stream);
    fclose(th_stdout_stream);

    stdout = th_real_stdout;
    th_stdout_stream = NULL;

    /* th_stdout_buf now holds the captured text (malloc'd by
       open_memstream); ownership passes to the caller. */
    char *result = th_stdout_buf;
    th_stdout_buf = NULL;
    th_stdout_size = 0;

    return result != NULL ? result : strdup("");
}

char *th_call_capturing(void (*fn)(void))
{
    th_capture_stdout_start();
    fn();
    return th_capture_stdout_end();
}

/* ---- temp working directory ------------------------------------ */

static char th_prev_cwd[4096];
static char th_tmp_dir[4096];

static int th_rm_visitor(const char *path, const struct stat *sb,
                          int typeflag, struct FTW *ftwbuf)
{
    (void)sb; (void)ftwbuf;

    if(typeflag == FTW_DP || typeflag == FTW_D)
        rmdir(path);
    else
        unlink(path);

    return 0;
}

void th_enter_tmp_dir(void)
{
    if(getcwd(th_prev_cwd, sizeof(th_prev_cwd)) == NULL)
        th_prev_cwd[0] = '\0';

    strcpy(th_tmp_dir, "/tmp/df_test_XXXXXX");

    if(mkdtemp(th_tmp_dir) == NULL)
        return;

    if(chdir(th_tmp_dir) != 0)
    {
        /* leave prev_cwd recorded so leave_tmp_dir() is still safe */
    }
}

void th_leave_tmp_dir(void)
{
    if(th_prev_cwd[0] != '\0')
        chdir(th_prev_cwd);

    if(th_tmp_dir[0] != '\0')
        nftw(th_tmp_dir, th_rm_visitor, 16, FTW_DEPTH | FTW_PHYS);

    th_tmp_dir[0] = '\0';
}
