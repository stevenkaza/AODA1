/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#include <string.h>
#include "exception.h"
#include "mark.h"
#include "vcutil.h"
#include "util.h"

#define NUM_POS_TESTS 5
#define NUM_NEG_TESTS 8

// TODO: should I create a negative test that tests for case-sensitive VCARD? do in readVcard

#define NEG_ERROR_TEST_START( desc )                                    \
    START_NEG_TEST( ++negCtr, desc )                                    \
    vcard = NULL;                                                       \
                                                                        \
    if (getUnfoldedFails == 0) {                                        \
        try {                                                           \
            getUnfolded (NULL, NULL);                                   \
            /* they might fail a line number test if the above failed */ \
        } catch ( exception ) {                                         \
            getUnfoldedFails = 1;                                       \
        }                                                               \
    }                                                                   \

#define NEG_ERROR_TEST_STATUS( path, error )                            \
    vfile = fopen (path, "r");                                          \
    status = readVcard (vfile, &vcard);                                 \
    fclose (vfile);                                                     \
                                                                        \
    if ( status.code != error ) {                                       \
        LOG ("Invalid error return %s. Expected %s\n",                  \
             VCAM_VcError_toString (status.code),                       \
             VCAM_VcError_toString (error));                            \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    
#define NEG_ERROR_TEST( desc, path, error)                              \
    NEG_ERROR_TEST_START (desc);                                        \
                                                                        \
    try {                                                               \
        NEG_ERROR_TEST_STATUS (path, error);                            \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    PASS_NEG_TEST;                                                      \

#define NEG_ERROR_TEST_LINES( desc, path, error, lfrom, lto )           \
    NEG_ERROR_TEST_START (desc);                                        \
                                                                        \
    try {                                                               \
        NEG_ERROR_TEST_STATUS (path, error);                            \
                                                                        \
        if ( status.linefrom != lfrom && status.lineto != lto) {        \
            LOG ("Invalid line range [%d,%d].  Expected [%d,%d]\n",     \
                 status.linefrom, status.lineto, lfrom, lto);           \
            FAIL_NEG_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    PASS_NEG_TEST;

// TODO: open various test files for various cases, and run readVcFile ()

#define POS_CARD_TEST_START(desc)                                       \
    START_POS_TEST( ++posCtr, desc);                                    \
    vcard = NULL;                                                       \
                                                                        \
    if (getUnfoldedFails == 0) {                                        \
        try {                                                           \
            getUnfolded (NULL, NULL);                                   \
            /* they might fail a line number test if the above failed */ \
        } catch ( exception ) {                                         \
            getUnfoldedFails = 1;                                       \
        }                                                               \
    }                                                                   \

#define POS_CARD_TEST_STATUS(path, ecode, test_status)                  \
    vfile = fopen (path, "r");                                          \
    status = readVcard (vfile, &vcard);                                 \
                                                                        \
    if ( test_status && status.code != ecode ) {                        \
        LOG ("Invalid status returned: %s. Expected %s\n",              \
             VCAM_VcError_toString (status.code),                       \
             VCAM_VcError_toString (ecode));                            \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
    fclose (vfile);                                                     \


#define POS_CARD_TEST_SIMPLE(desc, path, numprops)   \
    POS_CARD_TEST_GENERIC(desc, path, OK, numprops, 0);    \

#define POS_CARD_TEST(desc, path, ecode, numprops)          \
    POS_CARD_TEST_GENERIC(desc, path, ecode, numprops, 1);    \

#define POS_CARD_TEST_GENERIC(desc, path, ecode, numprops, test_status) \
    POS_CARD_TEST_START(desc);                                          \
                                                                        \
    try {                                                               \
        POS_CARD_TEST_STATUS (path, ecode, test_status);                \
        if (vcard == NULL) {                                            \
            LOG ("Returned vcard was NULL.\n");                         \
            FAIL_POS_TEST;                                              \
        } else if (vcard->nprops != numprops) {                         \
            LOG ("Num props '%d' was not %d for the minimal file.\n", vcard->nprops, numprops); \
            FAIL_POS_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
                                                                        \
    PASS_POS_TEST;                                                      \
    

int s2( int suite, int weight, int argc, char *argv[] ) {
    VcStatus status;
    int exception;
//    int posCtr = 0;	(W14) now in mark.h
    int getUnfoldedFails = 0;
    Vcard *vcard;
    FILE *vfile;
    
    START_SUITE( suite, "readVcard", weight, NUM_POS_TESTS, NUM_NEG_TESTS );
    
    /********** TEST EMPTY FILE **********/
    POS_CARD_TEST_START ( "Test empty card file" );
    try {                                                               
        POS_CARD_TEST_STATUS ( "1_empty_file.vcf", OK, 1 );

        if (vcard != NULL) {                                            
            LOG ("vcard was not NULL to signify EOF when parsing empty file.\n"); 
            FAIL_POS_TEST;                                              
        }                                                               
    } catch ( exception ) {                                             
        LOG ("%s\n", Exception_Names[exception]);                       
        FAIL_POS_TEST;                                                  
    }                                                                   
    PASS_POS_TEST;                                                      

    /********** TEST MINIMAL FILE (N and FN) **********/
    POS_CARD_TEST_SIMPLE ("Test minimal card", "2_minimal_card_file.vcf", 2);

    /********** TEST RICH FILE (1 of each normal field) **********/
    POS_CARD_TEST ("Test rich card (with params)", "2_rich_card.vcf", OK, 16);

    /********** TEST RICH FILE (~2 of each field) **********/
#ifdef A1MODE
    POS_CARD_TEST ("Test really rich card (with params, dups)", "2_really_rich_card.vcf", OK, 27);
#else
    POS_CARD_TEST ("Test really rich card (with params, dups, and other)", "2_really_rich_card_with_other.vcf", OK, 28);
#endif    
    
    /********** TEST PROPERTY ACCESS ***********/
    START_POS_TEST( ++posCtr, "Test property access via vcard");    

    VcProp eprop = { VCP_URL, NULL, NULL, "http://www.muffinfilms.com", NULL };
    VcProp aprop;
    try {
        // This will fail if previous test failed to obtain card's properties
        // It will also fail if they included VERSION or BEGIN as properties
        if (vcard != NULL) {
            aprop = (vcard->prop)[16];
            if (! VcProp_equal_simple (aprop, eprop)) {
                LOG ("Property %s did not match expected property of %s.\n",
                     VCAM_VcProp_toString (aprop), VCAM_VcProp_toString (eprop));
                FAIL_POS_TEST;
            }
        } else {
            LOG ("Returned vcard was NULL.  Could not test property.\n");
            FAIL_POS_TEST;
        }
    } catch ( exception ) {
        LOG ("%s\n", Exception_Names[exception]);
        FAIL_POS_TEST;
    }
    PASS_POS_TEST;


//    int negCtr = 0;	(W14) now in mark.h

    NEG_ERROR_TEST ("Test missing BEGIN", "2_no_begin.vcf", BEGEND);
    NEG_ERROR_TEST ("Test missing END", "2_no_end.vcf", BEGEND);
    NEG_ERROR_TEST ("Test error precedence (missing end over version)", "2_no_version_or_end.vcf", BEGEND);
    NEG_ERROR_TEST ("Test bad version", "2_bad_version.vcf", BADVER); // used to test _LINES=2,2 here, but doing it for VcFile instead
    NEG_ERROR_TEST ("Test no version", "2_no_version.vcf", NOPVER);
    NEG_ERROR_TEST ("Test misplaced version", "2_misplaced_version.vcf", NOPVER);
    NEG_ERROR_TEST ("Test missing FN", "2_no_fn.vcf", NOPNFN);
    NEG_ERROR_TEST ("Test missing N", "2_no_n.vcf", NOPNFN);

    END_SUITE
}
