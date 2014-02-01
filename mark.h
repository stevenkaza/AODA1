/* mark.h
   1/26/07 Revised for 2750 W07 A1
   2/10/07 Revised for 2750 W07 A2
   1/30/10 fixed PASS_NEG_TEST printing subtotal with wrong sign
   2/25/11 include new fork.h to run tests async.
           refactor POS_TEST and NEG_TEST common code from a*-suite*.c here
	   alter calculation of negative test weights
   1/29/12 Small revision for 2750 W12 A1; added posCtr & negCtr vars
*/
#ifndef MARK_H_
#define MARK_H_

/* A test suite has this calling sequence:
	suite: suite no. starting from 0
	weight: total marks available for this suite (max. 12)
	argc, argv: from mark command

   To be consistent, all numbering of sequences is from 0!
*/
typedef int Testsuite( int suite, int weight, int argc, char *argv[] );

/* Global variables (in mark.c) */

extern int SuiteWeight, TestFailed;
extern double PosTestWeight, PosTestMark, NegTestWeight, NegTestMark;

#include <stdio.h>
#include <math.h>
#include "fork.h"

/* Macros for use in test suites -- all print to stderr, which we expect
   the automark script to capture in a file

   Usage:
	START_SUITE
		POS/NEG_TEST (pass it desc, the custom test function, and data)
	      or
		START_POS/NEG_TEST
			any no. of FAIL_POS/NEG_TEST
		PASS_POS/NEG_TEST (just prints result if any failed)
		
		(more POS/NEG tests)
	END_SUITE
*/

#define START_SUITE( suite, name, weight, posTests, negTests ) \
  fprintf( stderr, "\n*** RUNNING TEST SUITE #%d: %s; %d tests with total weight = %d\n", \
	   suite, name, posTests, weight );				\
  SuiteWeight = weight;							\
  PosTestWeight = (double)weight/posTests;				\
  NegTestWeight = negTests>0 ? (double)weight / fmax(negTests/2,posTests) : 0; \
  PosTestMark = NegTestMark = 0.0;					\
  int posCtr, negCtr;							\
  posCtr = negCtr = 0;							\

#define START_POS_TEST( test, name ) \
    fprintf( stderr, "\n/--------- FUNCTIONAL TEST #F%d: %s\n", test, name ); \
    TestFailed = 0;

#define PASS_POS_TEST \
    if ( !TestFailed ) { \
        PosTestMark += PosTestWeight; \
        fprintf( stderr, "\\--------- PASSED! +%.2f (pos. subtotal +%.2f)\n", \
                PosTestWeight, PosTestMark ); \
    } else fprintf( stderr, "\\--------- Failed (pos. subtotal +%.2f)\n", PosTestMark );

#define FAIL_POS_TEST \
    TestFailed = 1;

#define START_NEG_TEST( test, name ) \
    fprintf( stderr, "\n/--------- ERROR TEST #E%d: %s\n", test, name ); \
    TestFailed = 0;

#define PASS_NEG_TEST \
    if ( !TestFailed ) { \
        fprintf( stderr, "\\--------- Passed (neg. subtotal -%.2f)\n", NegTestMark ); \
    } else { \
        NegTestMark += NegTestWeight; \
        fprintf( stderr, "\\--------- FAILED! -%.2f (neg. subtotal -%.2f)\n", \
                NegTestWeight, NegTestMark ); \
    }

#define FAIL_NEG_TEST \
    TestFailed = 1;

#define POS_TEST(pdesc, ptest_func, ptest_data)                         \
    START_POS_TEST(++posCtr, pdesc);                                    \
    if (PASS != test_async(posCtr, pdesc, ptest_func, (void*)&ptest_data)) { \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
    PASS_POS_TEST;                                                      \
    
#define NEG_TEST(pdesc, ptest_func, ptest_data)                         \
    START_NEG_TEST(++negCtr, pdesc);                                    \
    if (PASS != test_async(negCtr, pdesc, ptest_func, (void*)&ptest_data)) { \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    PASS_NEG_TEST;                                                      \
    

/* Convention re test suite return value:
   Mark is rounded up to one decimal place, and returned as *10 integer
*/
#define END_SUITE { \
    double suiteMark = PosTestMark - fmin(PosTestMark/2,NegTestMark); \
    int truncMark = suiteMark*10; \
    if ( suiteMark*10 > truncMark ) ++truncMark; \
    fprintf( stderr, "*** SUITE MARK: %.2f - min(%.2f, %.2f) = %.1f / %d\n", \
            PosTestMark, PosTestMark/2, NegTestMark, truncMark/10.0, \
            SuiteWeight ); \
    return truncMark; }

/* Additional helpful macros
*/

// floating point comparison of x & y within tolerance epsilon
#define FEQUALS( x, y, epsilon ) \
	(fabs((double)(x)-(double)(y))<(double)(epsilon))

// log something to the same stream used by test suite macros (stderr)
#define LOG(...) fprintf(stderr,__VA_ARGS__)

// copy a named file to the log
#define LOGFILE(fname) { \
    FILE* fp = fopen( fname, "r" ); \
    int ch; \
    if ( fp ) while ( EOF != (ch=fgetc(fp)) ) fputc( ch, stderr ); \
    fclose( fp ); }

#endif // MARK_H_
