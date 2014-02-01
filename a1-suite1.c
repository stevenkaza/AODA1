/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

// Updated for W14 A1 spec (WBG)

#include <string.h>
#include "exception.h"
#include "mark.h"
#include "vcutil.h"
#include "util.h"

#define NUM_POS_TESTS 5
#define NUM_NEG_TESTS 2

#define NEG_ERROR_TEST_START( desc )                                    \
    START_NEG_TEST( ++negCtr, desc )                                    \
    memset( &vcf, 0, sizeof( VcFile ) );                                \
    status.code = OK;                                                   \
                                                                        \
    if (getUnfoldedFails == 0) {                                        \
        try {                                                           \
            getUnfolded (NULL, NULL);                                   \
            /* they might fail a line number test if the above failed */ \
        } catch ( exception ) {                                         \
            getUnfoldedFails = 1;                                       \
        }                                                               \
    }                                                                   \

// should be called inside a try/catch, after a _START
#define NEG_ERROR_TEST_STATUS( path, error, mode )                       \
    vfile = fopen (path, mode);                                         \
    status = readVcFile (vfile, &vcf);                                  \
    if (fcloseFails == 0) {                                             \
        try {                                                           \
            fclose (vfile);                                             \
        } catch ( exception ) {                                         \
            fcloseFails = 1;                                            \
            FAIL_NEG_TEST; /* we will only fail this once thanks to the flag */ \
        }                                                               \
    }                                                                   \
                                                                        \
    if ( status.code != error ) {                                       \
        LOG ("Invalid error return %s. Expected %s\n",                  \
             VCAM_VcError_toString (status.code),                       \
             VCAM_VcError_toString (error));                            \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    
// can be called by itself
#define NEG_ERROR_TEST( desc, path, error, mode)                        \
    NEG_ERROR_TEST_START (desc);                                        \
                                                                        \
    try {                                                               \
        NEG_ERROR_TEST_STATUS (path, error, mode);                       \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    PASS_NEG_TEST;                                                      \

// can be called by itself
#define NEG_ERROR_TEST_LINES( desc, path, error, lfrom, lto )           \
    NEG_ERROR_TEST_START (desc);                                        \
                                                                        \
    try {                                                               \
        NEG_ERROR_TEST_STATUS (path, error, "r");                        \
                                                                        \
        if ( status.linefrom != lfrom || status.lineto != lto) {        \
            LOG ("Invalid line range [%d,%d].  Expected [%d,%d]\n",     \
                 status.linefrom, status.lineto, lfrom, lto);           \
            FAIL_NEG_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    PASS_NEG_TEST;

#define POS_FILE_TEST_SIMPLE( desc, path, num_cards )       		\
    POS_FILE_TEST_GENERIC(desc, path, OK, 0, 0, num_cards, 0, 0);
    
#define POS_FILE_TEST( desc, path, estatus, num_cards )         	\
    POS_FILE_TEST_GENERIC(desc, path, estatus, 0, 0, num_cards, 1, 0);

#define POS_FILE_TEST_LINES( desc, path, efrom, eto, num_cards )        \
    POS_FILE_TEST_GENERIC(desc, path, OK, efrom, eto, num_cards, 0, 1);
    
#define POS_FILE_TEST_GENERIC( desc, path, estatus, efrom, eto, num_cards, test_status, test_lines ) \
    START_POS_TEST( ++posCtr, desc);                                    \
    memset( &vcf, 0, sizeof( VcFile ) );                                \
                                                                        \
    if (getUnfoldedFails == 0) {                                        \
        try {                                                           \
            getUnfolded (NULL, NULL);                                   \
            /* they might fail a line number test if the above failed */ \
        } catch ( exception ) {                                         \
            getUnfoldedFails = 1;                                       \
        }                                                               \
    }                                                                   \
                                                                        \
    try {                                                               \
        vfile = fopen (path, "r");                                      \
        status = readVcFile (vfile, &vcf);                              \
        if (fcloseFails == 0) {                                         \
            try {                                                       \
                fclose (vfile);                                         \
            } catch ( exception ) {                                     \
                fcloseFails = 1;                                        \
                FAIL_POS_TEST; /* we will only fail this once thanks to the flag */ \
            }                                                           \
        }                                                               \
                                                                        \
        if ( test_status && status.code != estatus ) {                  \
            LOG ("Invalid status returned: %s. Expected %s\n",          \
                 VCAM_VcError_toString (status.code),                   \
                 VCAM_VcError_toString (estatus));                      \
            FAIL_POS_TEST;                                              \
        }                                                               \
                                                                        \
        if ( test_lines && (status.linefrom != efrom || status.lineto != eto ) ) { \
            LOG ("Invalid line nos. returned: %d/%d. Expected %d/%d\n", \
                 status.linefrom, status.lineto, efrom, eto);           \
            FAIL_POS_TEST;                                              \
        }                                                               \
                                                                        \
	if (vcf.ncards != num_cards) {                                  \
            LOG ("Num cards '%d' was not %d for the given file.\n", vcf.ncards, num_cards); \
            FAIL_POS_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
                                                                        \
    PASS_POS_TEST;                                                      


int s1( int suite, int weight, int argc, char *argv[] ) {
    //VcError e;
    VcStatus status;
    int exception;
//    int posCtr = 0;	(W14) now in mark.h
    VcFile vcf;
    FILE *vfile;
    int getUnfoldedFails = 0;
    int fcloseFails = 0; // because the student closed the file on me? >:(

    START_SUITE( suite, "readVcFile", weight, NUM_POS_TESTS, NUM_NEG_TESTS );
    

    /********** TEST EMPTY FILE **********/
    POS_FILE_TEST( "Test empty file", "1_empty_file.vcf", OK, 0);

    /********** TEST SINGLE CARD **********/
    // being generous by not checking status on this one, rather than the 178 card one, which is more likely to fail anyway :)
    POS_FILE_TEST_SIMPLE( "Test single card file", "1_single_card_file.vcf", 1);

    /********** TEST PROPERTY ACCESS ***********/
    START_POS_TEST( ++posCtr, "Test property access via file");

    VcProp eprop = { VCP_TITLE, NULL, NULL, "Kerikyu", NULL };
    VcProp aprop;
    try {
        // use the vcf left over from the above test
        aprop = ((vcf.cardp)[0]->prop)[7];
        if (! VcProp_equal_simple (aprop, eprop)) {
            LOG ("Property %s did not match expected property of %s.\n",
                 VCAM_VcProp_toString (aprop), VCAM_VcProp_toString (eprop));
            FAIL_POS_TEST;
        }
    } catch ( exception ) {
        LOG ("%s\n", Exception_Names[exception]);
        FAIL_POS_TEST;
    }
    PASS_POS_TEST;

    /********** TEST SINGLE CARD LINE NOS. **********/
    POS_FILE_TEST_LINES( "Test line nos. for single card file", "1_single_card_file.vcf", 13, 13, 1);
    
    /********** TEST MANY CARD **********/
    POS_FILE_TEST( "Test many card file", "1_many_card_file.vcf", OK, 178);



//    int negCtr = 0;	(W14) now in mark.h

    /********** TEST SYNTAX **********/
    NEG_ERROR_TEST_LINES ("Test parseVcProp error propagation (SYNTAX and line nums)", "1_bad_syntax.vcf", SYNTAX, 11, 11);

    /********** TEST NOPVER **********/
    NEG_ERROR_TEST ("Test readVcard error propagation (NOPVER)", "1_no_version.vcf", NOPVER, "r");

    END_SUITE
}
