/* flags.h

This is a collection of user-modifiable flags used by automarker
utilities.  If you want to change the defaults, copy this file
to the automarker directory and customize it
*/
#ifndef FLAGS_H
#define FLAGS_H

/* If the automarker has to be revised after A1, this restores original behaviour */
// #define A1MODE


// Flags relevant to util.h/c

/* Enables logging of details of tests, even when you pass them.  Should be defined, but set to 0 to disable. */
#define LOGPASS 0
// #define NOISY 


// Flags relevant to fork.h/c

/* TIMEOUT: The number of seconds to generally wait for a test to
   complete.  Generally, most student functions should be done very
   quickly, but this could vary depending on assignment.  If you need
   to override this for a single test,q create a custom
   NEG_TEST/POS_TEST (which calls test_async) for the case you need,
   and have it call test_async_timeout () instead with an overriding
   value. (e.g. for Valgrind runs) */
#define TIMEOUT 4

/* SINGLE_PROCESS: Disables forking.  You can use this when debugging
   to do it all in a single process.  Generally leave as undefined, so
   we can be multiprocess and so the automarker won't die along with
   ill-fated student code. */
// #define SINGLE_PROCESS

/* TEST_FORK: enables some functions that just test the fork API to make
   sure it's working as expected. Defines a main () */
// #define TEST_FORK

#endif
