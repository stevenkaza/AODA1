/* fork.h

*/
#ifndef FORK_H
#define FORK_H

#include "flags.h"

/***
 * This provides methods that can safely run tests in a separate,
 * forked process and be immune to crashes encountered in the tests.
 *
 * These method take some identifying information about a test (for
 * printing, including a test number and a test name), as well as the
 * function that defines the test, and a void * to any data you need
 * to pass to the function.  The function has to match the prototype
 * of TestFunc, and only returns TestSuccess (though you could modify
 * the contents of the user_data).
 *
 * NOTE: for debugging code, it can be nice to DISABLE forking into
 * new processes, so you can more easily diagnose a problem in a
 * single process.  To do that, define SINGLE_PROCESS in flags.h.
 * However, in general it's nice to run in multi-process mode so that
 * the automarker won't be brought down by student bugs.
 */

/* TIMEOUT: The number of seconds to generally wait for a test to
   complete.  Generally, most student functions should be done very
   quickly, but this could vary depending on assignment.  If you need
   to override this for a single test,q create a custom
   NEG_TEST/POS_TEST (which calls test_async) for the case you need,
   and have it call test_async_timeout () instead with an overriding
   value. (e.g. for Valgrind runs) */


typedef enum {
  PASS = 10, FAIL, OTHER
} TestSuccess;

/* Signature for test functions passed to test_async */
typedef TestSuccess (*TestFunc)(void *user_data);

/* Following functions are described in fork.c */
TestSuccess test_async (int test_num, char *test_name, TestFunc test_fp, void *test_data);
TestSuccess test_async_timeout (int test_num, char *test_name, TestFunc test_fp, void *test_data, int timeout);

#endif
