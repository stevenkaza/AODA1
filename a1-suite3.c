/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#include <string.h>
#include "exception.h"
#include "mark.h"
#include "vcutil.h"
#include "util.h"

#define NUM_POS_TESTS 13
#define NUM_NEG_TESTS 2

// should be called at the start of a test
#define NEG_ERROR_TEST_START( desc )                                    \
    START_NEG_TEST( ++negCtr, desc )                                    \
    line = NULL;                                                        \
                                                                        \
    if (getUnfoldedFails == 0) {                                        \
        try {                                                           \
            getUnfolded (NULL, NULL);                                   \
            /* they might fail a line number test if the above failed */ \
        } catch ( exception ) {                                         \
            getUnfoldedFails = 1;                                       \
        }                                                               \
    }                                                                   \

// should be called in a try/catch, after a _START
#define NEG_ERROR_TEST_STATUS(path, error, numskip)                     \
    vfile = fopen (path, "r");                                          \
    for (int i = 0; i < numskip; i++) {                                 \
        getUnfolded (vfile, &line);                                     \
    }                                                                   \
    status = getUnfolded (vfile, &line);                                \
                                                                        \
    if ( status.code != error ) {                                       \
        LOG ("Invalid error return %s. Expected %s\n",                  \
             VCAM_VcError_toString (status.code),                       \
             VCAM_VcError_toString (error));                            \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
    fclose (vfile);                                                     \
    
// can be called by itself
#define NEG_ERROR_TEST( desc, path, error, eline, numskip)              \
    NEG_ERROR_TEST_START (desc);                                        \
                                                                        \
    try {                                                               \
        NEG_ERROR_TEST_STATUS (path, error, numskip);                   \
                                                                        \
        if (! string_equal (line, eline)) {                             \
            qline = string_toQuotableString (line);                     \
            qeline = string_toQuotableString (eline);                   \
            LOG ("Line %s did not match expected %s.\n", qline, qeline); \
            free (qline);                                               \
            free (qeline);                                              \
            FAIL_NEG_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("Output crashed string comparison.\n");                    \
        FAIL_NEG_TEST;                                                  \
    }                                                                   \
                                                                        \
    PASS_NEG_TEST;                                                      \

// can be called by itself
#define NEG_ERROR_TEST_LINES( desc, path, error, lfrom, lto, numskip )  \
    NEG_ERROR_TEST_START (desc);                                        \
                                                                        \
    try {                                                               \
        NEG_ERROR_TEST_STATUS (path, error, numskip);                   \
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
                                                                        \
    PASS_NEG_TEST;                                                      \
    
// should to be called at the start of a test
#define POS_LINE_TEST_START(desc)                                       \
    START_POS_TEST( ++posCtr, desc);                                    \
    line = NULL;                                                        \
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
#define POS_LINE_TEST_STATUS(path, ecode, numskip)                       \
    vfile = fopen (path, "r");                                          \
    for (int i = 0; i < numskip; i++) {                                 \
        getUnfolded (vfile, &line);                                     \
    }                                                                   \
    status = getUnfolded (vfile, &line);                                \
                                                                        \
    if ( status.code != ecode ) {                                       \
        LOG ("Invalid status returned: %s. Expected %s\n",              \
             VCAM_VcError_toString (status.code),                       \
             VCAM_VcError_toString (ecode));                            \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
    fclose (vfile);                                                     \

// should be called in a try/catch after _START, after a call to getUnfolded
#define POS_LINE_TEST_LINES_STATUS(elinefrom, elineto)                  \
    if (status.linefrom != elinefrom || status.lineto != elineto) {     \
        LOG ("Reported line range [%d,%d] did not match expected line range [%d,%d].\n", \
             status.linefrom, status.lineto, elinefrom, elineto);       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
    
// can be called by itself
#define POS_LINE_TEST(desc, path, ecode, eline, numskip)                \
    POS_LINE_TEST_START(desc);                                          \
                                                                        \
    try {                                                               \
        POS_LINE_TEST_STATUS(path, ecode, numskip);                     \
                                                                        \
        if (! string_equal (line, eline)) {                             \
            qline = string_toQuotableString (line);                     \
            qeline = string_toQuotableString (eline);                   \
            LOG ("Line %s did not match expected %s.\n", qline, qeline); \
            free (qline);                                               \
            free (qeline);                                              \
            FAIL_POS_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
                                                                        \
    PASS_POS_TEST;                                                      \
    
// can be called by itself
#define POS_LINE_TEST_LINES(desc, path, ecode, elinefrom, elineto, numskip) \
    POS_LINE_TEST_START(desc);                                          \
                                                                        \
    try {                                                               \
        POS_LINE_TEST_STATUS(path, ecode, numskip);                     \
                                                                        \
        POS_LINE_TEST_LINES_STATUS (elinefrom, elineto);                \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
    PASS_POS_TEST;                                                      \
    
int s3( int suite, int weight, int argc, char *argv[] ) {
    VcStatus status;
    int exception;
//    int posCtr = 0;	(W14) now in mark.h
    char *line;
    FILE *vfile;
    char *qline;
    char *qeline;
    int getUnfoldedFails = 0;
    
    START_SUITE( suite, "getUnfolded", weight, NUM_POS_TESTS, NUM_NEG_TESTS );
    
    // We're implicitly testing that \r\n is stripped, and that it ends with \0

    /********** LINE NUMBER TESTS (x2) **********/
    POS_LINE_TEST_LINES("Test read line 2", "3_line_number.vcf", OK, 2, 2, 1);
    POS_LINE_TEST_LINES("Test read folded lines 3-8", "3_line_number_folded.vcf", OK, 3, 8, 2);

    /********** TEST LINE RESET **********/
    POS_LINE_TEST_START("Test line count reset to 0");
    try {
        POS_LINE_TEST_STATUS("3_single_line.vcf", OK, 3); // opens, reads, closes
        status = getUnfolded (NULL, NULL);
        POS_LINE_TEST_LINES_STATUS(0, 0);
    } catch ( exception ) {
        FAIL_POS_TEST;
    }
    PASS_POS_TEST;

    /********** TEST NO STATIC BUFFER **********/
    // want to make sure that they don't return a static buffer, so comparing addresses
    // will call getUnfolded () twice (without freeing yet) and make sure that they have
    // different values
    START_POS_TEST( ++posCtr, "Test buffer uniqueness (don't return static buffers)");
    line = NULL;
    char *line2 = NULL;

    try {
        vfile = fopen ("3_line_number.vcf", "r"); 
        getUnfolded (vfile, &line);
        getUnfolded (vfile, &line2);

        if (line == NULL && line2 == NULL) {
            LOG ("Couldn't confirm distinct buffers, saw NULL.\n");
            FAIL_POS_TEST;
        } else if (string_equal (line, line2)) {
            LOG ("Used a static buffer, overwriting previous line.\n");
            FAIL_POS_TEST;
        }

        fclose (vfile);
    } catch ( exception ) {
        LOG ("%s\n", Exception_Names[exception]);                       
        FAIL_POS_TEST;                                                  
    }
    PASS_POS_TEST 


    /********** LINE CONTENT/SPACE/EOF TESTS (x7) **********/
    POS_LINE_TEST("Test single line", "3_single_line.vcf", OK, "Supercalifragilisticexpialidocious, if you say it really fast,", 0);
    POS_LINE_TEST("Test single line with a space at the end", "3_single_line_space.vcf", OK, "Supercalifragilisticexpialidocious, if you say it really fast, ", 0);

    POS_LINE_TEST("Test skip blank line then read verse", "3_blank_line_skip.vcf", OK, "His wings are gray and trailing, / Azrael, Angel of Death. / And yet the souls that Azrael brings /  Across the dark and cold, / Look up beneath those folded wings, / And find them lined with gold - Robert Welsh", 0);
    POS_LINE_TEST("Test empty line with EOF", "3_lone_eof.vcf", OK, NULL, 1);  
    POS_LINE_TEST("Test simple folded line (space)", "3_line_folded_space.vcf", OK, "BEGIN:VCARDEND:VCARD", 0);
    POS_LINE_TEST("Test simple folded line (tab)", "3_line_folded_tab.vcf", OK, "BEGIN:VCARDEND:VCARD", 0);
    POS_LINE_TEST("Test folded line (with two spaces, preserve second one)", "3_line_folded_double_space.vcf", OK, "BEGIN:VCARD END:VCARD", 0);

    int photolen = 19650;
    char *start = "PHOTO:/9j/4AAQSkZJRgABAQEASAB";
    char *end = "rlSUj/SP+eOauxdiPqsVj//Z";

    /********** TEST MANY FOLDED LINE (photo) **********/
    POS_LINE_TEST_START ("Test many folded line (270 line photo)");
    try {                                                               
        POS_LINE_TEST_STATUS ("3_photo.vcf", OK, 0);
        if (! string_heuristic_equal (photolen, start, end, line)) {
            FAIL_POS_TEST;
        }
    } catch ( exception ) {                                             
        LOG ("%s\n", Exception_Names[exception]);                       
        FAIL_POS_TEST;                                                  
    }                                                                   
    PASS_POS_TEST;                                                      
    
    /********** TEST VERY LONG LINE (not-folded) **********/
    POS_LINE_TEST_START ("Test very long line (19650 char photo, not folded)");
    try {
        POS_LINE_TEST_STATUS ("3_photo_single_line.vcf", OK, 0);
        if (! string_heuristic_equal (photolen, start, end, line)) {
            FAIL_POS_TEST;                                              
        }
    } catch ( exception ) {                                             
        LOG ("%s\n", Exception_Names[exception]);                       
        FAIL_POS_TEST;                                                  
    }
    PASS_POS_TEST;



//    int negCtr = 0;	(W14) now in mark.h

    /********** TEST "Y<EOF>" (x2) **********/
    NEG_ERROR_TEST ("Test non-empty line with EOF - 1st read: line", "3_line_eof.vcf", OK, "END:VCARD", 0);
    NEG_ERROR_TEST ("Test non-empty line with EOF - 2nd read: EOF's NULL", "3_line_eof.vcf", OK, NULL, 1); 

    END_SUITE
}
