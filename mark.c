/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#include <stdlib.h>
#include "exception.h"
#include "mark.h"

/* Global variables used by marking.h macros */

int SuiteWeight, TestFailed;
double PosTestWeight, PosTestMark, NegTestWeight, NegTestMark;

//--------------------------
//  declare your test suite functions here
Testsuite s1;
Testsuite s2;
Testsuite s3;
Testsuite s4;
Testsuite s5;
Testsuite s6;
Testsuite s7;

/* HARD LIMITS due to 8-bit capacity of main() return:
	- each suite weight <= 12
	- total suite weights <= 120 (i.e., 10x max. suite weight)
  [0] is not used, leave as placeholder
*/
Testsuite *psuite[] = { 0, s1, s2, s3, s4, s5 };
int weight[] = { 0, 6, 8, 8, 12, 2 };
//--------------------------

// below here is standard
int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf( stderr, "*** Usage: ./mark <suite no.>\n" );
    }

    int suiteNo = atoi( argv[1] );		// get suite no. from 1st arg
    int numSuites = sizeof(weight)/sizeof(int) - 1;

    // suite no. "0" means return total weight of test suites
    if ( suiteNo == 0 ) {
        int wt = 0;
        for ( int i=1; i<=numSuites; i++ ) {
            if ( weight[i] > 12 )
                fprintf( stderr, "*** A suite weight > 12!\n" );
            wt+=weight[i];
        }
        if ( wt > 120 )
            fprintf( stderr, "*** Suite weights total > 120!\n" );
        return wt;
    }

    // suite no. out of range => no more suites to run
    if ( suiteNo > numSuites ) {
        return 121; // invalid suite no.
    }

    // set up exception handler and call suite
    init_exception_handler(argv[0]);	// passing prog name
    return (*psuite[suiteNo])( suiteNo, weight[suiteNo], argc, argv );
}
