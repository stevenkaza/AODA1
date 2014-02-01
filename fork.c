/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#define _POSIX_C_SOURCE 199309L

#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "exception.h"
#include <time.h>
#include <string.h>
#include "fork.h"

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
 * single process.  To do that, either define SINGLE_PROCESS
 * (e.g. using the flag in fork.h) at compile time or set an AMFORK
 * environment variables to 0.  However, in general it's nice to run
 * in multi-process mode so that the automarker won't be brought down
 * by student bugs.
 */

/*** library functions ***/

/* convenience array of string constants so we can print out whether a
   test passed, failed, etc. using the enumerated values as passed to
   TestSuccess_to_string */
char *TestSuccessStrings[] = { "PASS", "FAIL", "OTHER" };

/**
 * TestSuccess_to_string:
 *
 * Takes a TestSuccess enum value (PASS, FAIL, OTHER) and returns the
 * string constant for that value.  No allocating required!
 */
char *TestSuccess_to_string (TestSuccess ts) {
  return TestSuccessStrings[ts-10];
}

/**
 * run_test:
 *
 * A small little wrapper for the user-provided test functions that
 * will get run in a new process.  Also prints exceptions.
 */
static TestSuccess run_test (TestFunc test_fp, void *test_data) {
  int result;
  int exception;

  try {
    result = test_fp (test_data);
  } catch ( exception ) {
    fprintf (stderr, "Exception [%s]\n", Exception_Names[exception]);
    // printStackTrace (); // can't get the address of their function
    result = FAIL;
  }

  return result;
}

/**
 * manage_test:
 *
 * Private. Manages a running test, given its process ID and a timeout length.
 * It's a bit stupid, and polls the running test process once a
 * millisecond (most tests should take less time than that).  If the
 * test process doesn't return before the timeout, the test process is
 * killed and faliure is reported.  If it does return in time, we
 * return the exit value of the process (which will be the return
 * value of the test function the test process was executing).  Note
 * that if SINGLE_PROCESS is defined, manage_test is not used.
 *
 * pid: The process ID of the process running the test function.
 * timeout: the number of seconds to wait before aborting the test process.
 *
 * returns: The exit status of the test process or FAIL if it times out. 
 */
static TestSuccess manage_test (pid_t pid, int timeout) { 
    pid_t wait_pid;
    int wait_status;
    struct timespec duration = { 0, 1000000 }; // = { 0, 1000000 }; // 10^6ns=1ms
    int i;
    TestSuccess result = OTHER;

    /* Check every few milliseconds to see if we're done yet, but don't
       wait.  We want a chance to cancel it if we wait too long. */
    for (i = 0; 1; i++) {
        nanosleep (&duration, NULL);
        wait_pid = waitpid (pid, &wait_status, WNOHANG);
    
        if (wait_pid != 0) {
            /* Test is returned */

            if (WIFEXITED(wait_status)) {
                /* The test exited normally and find out its result. */
                result = WEXITSTATUS (wait_status);
                /* printf ("parent result: %d, wait_status: %d, wait_pid: %d\n", result, wait_status, wait_pid); */
                break;
            } else {
                /* The test has ended abnormally */
                fprintf (stderr, "Test did not exit normally, was signalled by [%d].  Assuming failed.\n", WTERMSIG (wait_status));
                result = FAIL;
                break;
            }

        } else if (i >= timeout * 1000) {
            /* We're not done after seconds, give up */
            fprintf (stderr, "Test is taking too long. Aborting as failed.\n");
            kill (pid, SIGTERM); // SIGTERM to prevent zombie apocalypse
            waitpid (pid, &wait_status, 0); // wait for it to finish dying
        }
    }
  
  /* printf ("parent: waited on [%d], got exit status [%d]; thread was [%d]\n", wait_pid, status, pid); */
  /* fprintf (stderr, "parent: test Exception [%s]\n", Exception_Names[exception]); */

  return result;
}

/**
 * test_async_timeout:
 * This is like test_async (), except that you can specify the timeout interval in seconds.
 *
 * test_num: a number identifying the test, for reporting.
 * test_name: a name describing the test.
 * test_fp: a function pointer to a function implementing a test.
 * test_data: a void * pointer to data passed to test_fp.
 * timeout: the maximum number of seconds to wait for test_fp to execute before aborting it as failed.
 *
 * returns: the return value from test_fp, which is TestSuccess, so either PASS, FAIL, or OTHER.
 */
TestSuccess test_async_timeout (int test_num, char *test_name, TestFunc test_fp, void *test_data, int timeout) {
#ifdef TEST_FORK
    printf ("========================================\n");
    printf ("Test %d %s: Start\n", test_num, test_name);
#endif

    char *amfork = getenv ("AMFORK");
#ifdef SINGLE_PROCESS
    amfork = "0";
#endif    
    
    if (amfork != NULL && 0 == strcmp (amfork, "0")) {
        return run_test (test_fp, test_data);
    } else {
        pid_t pid;
        int result;
        
        pid = fork();
        if (pid == 0) {
            /* we're the child, run the test and exit with the result */
            result = run_test (test_fp, test_data);
            exit (result);
        } else {
            /* we're the parent, manage the running test and return its result */
            result = manage_test (pid, timeout);
        
#ifdef TEST_FORK
            printf ("Test %d %s: Result %s\n", test_num, test_name, TestSuccess_to_string (result));
#endif    
            
            return result;
        }
    }
}

/**
 * test_async:
 * This function will run a test (defined in a function) in safe way, immune to crashes caused
 * by the test.  You pass it some identifying information, such as a test number and a test
 * name (mostly used for reporting), as well as a function pointer to the test function, and
 * optionally a void * to any data you want to send to the test function.  The test function
 * must match the TestFunc prototype, and only returns TestSuccess.  This function returns
 * that TestSuccess.
 *
 * test_num: a number identifying the test, for reporting.
 * test_name: a name describing the test.
 * test_fp: a function pointer to a function implementing a test.
 * test_data: a void * pointer to data passed to test_fp.
 * timeout: the maximum number of seconds to wait for test_fp to execute before aborting it as failed.
 *
 * returns: the return value from test_fp, which is TestSuccess, so either PASS, FAIL, or OTHER.
 */
TestSuccess test_async (int test_num, char *test_name, TestFunc test_fp, void *test_data) {
    return test_async_timeout (test_num, test_name, test_fp, test_data, TIMEOUT);
}

/***
 * Here are some tests for the above code, to make sure it's behaving
 * as expected.
 */

#ifdef TEST_FORK

/*** mock tests ***/
TestSuccess hello (void *test_data) {
  char *target = (char*) test_data;
  int size = strlen (target) + 1;
  char *input = malloc (sizeof (char) * size);
  fprintf (stdout, "Type '%s'> ", target);
  input = fgets (input, size, stdin);
  if (strncmp (target, input, size) == 0) {
    return PASS;
  } else {
    return FAIL;
  }  
}

TestSuccess fail (void *test_data) {
  return FAIL;
}
TestSuccess pass (void *test_data) {
  return PASS;
}

// memory issue
TestSuccess foo (void *test_data) {
  char *willy;
  willy = malloc (sizeof (char));
  free (willy);
  free (willy);
  return PASS;
}

// infinite loop 
TestSuccess bar (void *test_data) {
  while (1) {
  }
  return PASS;
}

/*** test main ***/

int main (int argc, char** argv) {
  init_exception_handler(argv[0]);

  test_async (1, "Bar Infinite loop", bar, NULL);
  test_async (2, "Pass", pass, NULL);
  test_async (3, "Foo memory corruption", foo, NULL);
  test_async (4, "Fail", fail, NULL);
  test_async (4, "User Input == 'HELLO'", hello, "HELLO");

  return 17;
}

#endif 
