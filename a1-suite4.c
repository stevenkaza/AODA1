/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#include <string.h>
#include "exception.h"
#include "mark.h"
#include "vcutil.h"
#include "util.h"

#define NUM_POS_TESTS 20
#define NUM_NEG_TESTS 4

#define NEG_ERROR_TEST( desc, propstr, ecode)               \
    START_NEG_TEST( ++negCtr, desc )                        \
    memset (&prop, 0, sizeof (VcProp));                   \
                                                            \
    try {                                                   \
        error = parseVcProp (propstr, &prop);               \
                                                            \
        if ( error != ecode ) {                             \
            LOG ("Invalid error return %s. Expected %s\n",  \
                 VCAM_VcError_toString (error),             \
                 VCAM_VcError_toString (ecode));            \
            FAIL_NEG_TEST;                                  \
        }                                                   \
    } catch ( exception ) {                                 \
        LOG ("%s\n", Exception_Names[exception]);           \
        FAIL_NEG_TEST;                                      \
    }                                                       \
                                                            \
    PASS_NEG_TEST;                                          \


#define POS_PROP_TEST_START(desc, memval)                               \
    START_POS_TEST( ++posCtr, desc);                                    \
    memset (&prop, memval, sizeof (VcProp));                            \

// should be called in a try/catch, after _START
#define POS_PROP_TEST_STATUS(propstr, ecode)                            \
    error = parseVcProp (propstr, &prop);                               \
                                                                        \
    if ( error != ecode ) {                                             \
        LOG ("Invalid error returned: %s. Expected %s\n",               \
             VCAM_VcError_toString (error),                             \
             VCAM_VcError_toString (ecode));                            \
        FAIL_POS_TEST;                                                  \
    }                                                                   \

// complete
#define POS_PROP_TEST(desc, propstr, ecode, eprop)                      \
    POS_PROP_TEST_GENERIC(desc, propstr, ecode, eprop, 0);              \

#define POS_PROP_TEST_DIRTY(desc, propstr, ecode, eprop)                \
    POS_PROP_TEST_GENERIC(desc, propstr, ecode, eprop, 8);             \


#define POS_PROP_TEST_GENERIC(desc, propstr, ecode, eprop, memval)      \
    POS_PROP_TEST_START(desc, memval);                                  \
                                                                        \
    try {                                                               \
        POS_PROP_TEST_STATUS (propstr, ecode);                          \
                                                                        \
        if (! VcProp_equal (eprop, prop)) {                             \
            try {                                                       \
                /* try to stringify props for better explanation, but a really bad VcProp could trip us */ \
                vcprop_str = VCAM_VcProp_toString (prop);               \
                vcprop_str_expected = VCAM_VcProp_toString (eprop);     \
                /* Only expecting N and FN */                           \
                LOG ("VcProp '%s' did not match expected '%s' for '%s'.\n", vcprop_str, vcprop_str_expected, propstr); \
            } catch ( exception ) {                                     \
                LOG ("Parsed VcProp did not match expected '%s'.\n", propstr); \
            }                                                           \
            /* had a weird bug that really requires we free our memory here :) */ \
            if (vcprop_str != NULL) {                                   \
                free (vcprop_str);                                      \
                vcprop_str = NULL;                                      \
            }                                                           \
            if (vcprop_str != NULL) {                                   \
                free (vcprop_str_expected);                             \
                vcprop_str_expected = NULL;                             \
            }                                                           \
                                                                        \
            FAIL_POS_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
                                                                        \
    PASS_POS_TEST;                                                      
    
// complete
#define POS_PROP_TEST_SIMPLE(desc, propstr, ecode, eprop)               \
    POS_PROP_TEST_START(desc, 0);                                       \
                                                                        \
    try {                                                               \
        POS_PROP_TEST_STATUS(propstr, ecode);                           \
                                                                        \
        if (! VcProp_equal_simple (eprop, prop)) {                      \
            try {                                                       \
                vcprop_str = VCAM_VcProp_toString (prop);               \
                vcprop_str_expected = VCAM_VcProp_toString (eprop);     \
                /* Only expecting N and FN */                           \
                LOG ("VcProp '%s' did not match expected '%s' for '%s'.\n", vcprop_str, vcprop_str_expected, propstr); \
            } catch ( exception ) {                                     \
                LOG ("Parsed VcProp did not match expected '%s'.\n", propstr); \
            }                                                           \
            if (vcprop_str != NULL) {                                   \
                free (vcprop_str);                                      \
                vcprop_str = NULL;                                      \
            }                                                           \
            if (vcprop_str != NULL) {                                   \
                free (vcprop_str_expected);                             \
                vcprop_str_expected = NULL;                             \
            }                                                           \
                                                                        \
            FAIL_POS_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
                                                                        \
    PASS_POS_TEST;                                                      
    
// complete
#define POS_PROP_TEST_NAME(desc, propstr, ecode, eprop)                 \
    POS_PROP_TEST_START(desc, 0);                                       \
                                                                        \
    try {                                                               \
        POS_PROP_TEST_STATUS(propstr, ecode);                           \
                                                                        \
        if (! VcProp_equal_name (eprop, prop)) { \
            try {                                                       \
                vcprop_str = VCAM_VcProp_toString (prop);               \
                vcprop_str_expected = VCAM_VcProp_toString (eprop);     \
                /* Only expecting N and FN */                           \
                LOG ("VcProp '%s' did not match expected '%s'.\n", vcprop_str, vcprop_str_expected); \
            } catch ( exception ) {                                     \
                LOG ("Parsed VcProp did not match expected '%s'.\n", propstr); \
            }                                                           \
            if (vcprop_str != NULL) {                                   \
                free (vcprop_str);                                      \
                vcprop_str = NULL;                                      \
            }                                                           \
            if (vcprop_str != NULL) {                                   \
                free (vcprop_str_expected);                             \
                vcprop_str_expected = NULL;                             \
            }                                                           \
                                                                        \
            FAIL_POS_TEST;                                              \
        }                                                               \
    } catch ( exception ) {                                             \
        LOG ("%s\n", Exception_Names[exception]);                       \
        FAIL_POS_TEST;                                                  \
    }                                                                   \
                                                                        \
    PASS_POS_TEST;                                                      
    

int s4( int suite, int weight, int argc, char *argv[] ) {
    VcError error;
    int exception;
//    int posCtr = 0;	(W14) now in mark.h
    VcProp prop;

    char* vcprop_str = NULL;
    char* vcprop_str_expected = NULL;
    
    
    START_SUITE( suite, "parseVcProp", weight, NUM_POS_TESTS, NUM_NEG_TESTS );
    
    VcProp prop_begin = { VCP_BEGIN, NULL, NULL, "VCARD", NULL };
    POS_PROP_TEST_DIRTY("Test BEGIN:VCARD (and NULL fields)", "BEGIN:VCARD", OK, prop_begin);

    VcProp prop_end = { VCP_END, NULL, NULL, "VCARD", NULL };
    POS_PROP_TEST_NAME("Test END:VCARD name", "END:VCARD", OK, prop_end);

    VcProp prop_version = { VCP_VERSION, NULL, NULL, VCARD_VER, NULL };
    POS_PROP_TEST_SIMPLE("Test VERSION:3.0", "VERSION:3.0", OK, prop_version);

    VcProp prop_n = { VCP_N, NULL, NULL, "Gilmore;Leigh;Lorelai;;", NULL };
    POS_PROP_TEST_SIMPLE("Test N", "N:Gilmore;Leigh;Lorelai;;", OK, prop_n);

    VcProp prop_fn = { VCP_FN, NULL, NULL, "Rory Gilmore", NULL };
    POS_PROP_TEST_NAME("Test FN name", "FN:Rory Gilmore", OK, prop_fn);
    // 5

    VcProp prop_nickname = { VCP_NICKNAME, NULL, NULL, "Pineapple", NULL };
    POS_PROP_TEST_SIMPLE("Test NICKNAME", "NICKNAME:Pineapple", OK, prop_nickname);

    POS_PROP_TEST("Test NICKNAME discarding group", "group1.NICKNAME:Pineapple", OK, prop_nickname);

    VcProp prop_photo = { VCP_PHOTO, NULL, NULL, "0123456789ABCDEF", NULL };
    POS_PROP_TEST_NAME("Test PHOTO name", "PHOTO:0123456789ABCDEF", OK, prop_photo);

    VcProp prop_bday = { VCP_BDAY, NULL, NULL, "1984-10-08", NULL };
    POS_PROP_TEST_SIMPLE("Test BDAY", "BDAY:1984-10-08", OK, prop_bday);

    VcProp prop_adr = { VCP_ADR, "HOME", NULL, ";;37 Maple St.;Stars Hollow;CT", NULL };
    POS_PROP_TEST_DIRTY("Test ADR, TYPE, and memory safety", "ADR;TYPE=HOME:;;37 Maple St.;Stars Hollow;CT", OK, prop_adr);
    // 10

    VcProp prop_label = { VCP_LABEL, NULL, NULL, "Student", NULL };
#ifdef A1MODE
    POS_PROP_TEST("Test LABEL and CHARSET ignore", "LABEL;CHARSET=utf8:Student", OK, prop_label);
#else
    /* A2's spec adds ENCODING to list of parameters we should ignore,
       doubling up into a single ignore test */
    POS_PROP_TEST("Test LABEL, and CHARSET & ENCODING ignore", "LABEL;CHARSET=utf8;ENCODING=Klingon:Student", OK, prop_label);
#endif

    VcProp prop_tel = { VCP_TEL, "CELL", NULL, "1-555-123-4567", NULL };
    POS_PROP_TEST("Test TEL and case-insensitive parameter", "TEL;Type=CELL:1-555-123-4567", OK, prop_tel);

    VcProp prop_email = { VCP_EMAIL, "SCHOOL,INTERNET", "YALE.EDU", "rory.gilmore@yale.edu", NULL };
#ifdef A1MODE
    POS_PROP_TEST("Test EMAIL, TYPE, and VALUE", "EMAIL;TYPE=SCHOOL,INTERNET;VALUE=YALE.EDU:rory.gilmore@yale.edu", OK, prop_email);
#else
    /* A2's spec wants multiple types to be joined */
    POS_PROP_TEST("Test EMAIL, double TYPE, and VALUE", "EMAIL;TYPE=SCHOOL;TYPE=INTERNET;VALUE=YALE.EDU:rory.gilmore@yale.edu", OK, prop_email);
#endif

    VcProp prop_geo = { VCP_GEO, NULL, NULL, "41.7639,-72.6873", NULL };
    POS_PROP_TEST_SIMPLE("Test GEO", "GEO:41.7639,-72.6873", OK, prop_geo);
    
    VcProp prop_title = { VCP_TITLE, NULL, NULL, "Student", NULL };
    POS_PROP_TEST_NAME("Test TITLE name", "TITLE:Student", OK, prop_title);
    // 15

    VcProp prop_org = { VCP_ORG, NULL, NULL, "Yale", NULL };
    POS_PROP_TEST_SIMPLE("Test ORG and insensitive pname", "Org:Yale", OK, prop_org);

    VcProp prop_note = { VCP_NOTE, NULL, NULL, "I should have gone to Harvard.\\nOr maybe just worked as a cashier like Kirk?", NULL };
    POS_PROP_TEST_SIMPLE("Test NOTE and \\n", "NOTE:I should have gone to Harvard.\\nOr maybe just worked as a cashier like Kirk?", OK, prop_note);
    
    VcProp prop_uid = { VCP_UID, NULL, NULL, "FEDCBA9876543210", NULL };
    POS_PROP_TEST_NAME("Test UID name", "UID:FEDCBA9876543210", OK, prop_uid);
    
    VcProp prop_url = { VCP_URL, NULL, NULL, "http://www.yale.edu/~rory.gilmore/", NULL };
    POS_PROP_TEST_SIMPLE("Test URL", "URL:http://www.yale.edu/~rory.gilmore/", OK, prop_url);

#ifdef A1MODE
    VcProp prop_other = { VCP_OTHER, NULL, NULL, NULL, NULL };
    POS_PROP_TEST_SIMPLE("Test OTHER", "BESTFRIEND:Lane Kim", OK, prop_other);
#else
    VcProp prop_other = { VCP_OTHER, NULL, NULL, "BESTFRIEND:Lane Kim", NULL };
    POS_PROP_TEST("Test OTHER", "BESTFRIEND:Lane Kim", OK, prop_other);
#endif    
    // 20

//    int negCtr = 0;	(W14) now in mark.h

#ifdef A1MODE    
    NEG_ERROR_TEST("Test PAROVER: two types", "EMAIL;TYPE=INTERNET;TYPE=WORK:gilmore@nytimes.com", PAROVER);

    NEG_ERROR_TEST("Test PAROVER: ENCODING=...", "ADR;ENCODING=QUOTED:;;37 Maple St.;Stars Hollow;CT", PAROVER);
#else
    /* A2 spec changed behaviour, test that students followed */
    NEG_ERROR_TEST("Test PAROVER: nonsense parameters", "EMAIL;MEDIUM=INTERNET;SUBJUGATION=WORK:gilmore@nytimes.com", PAROVER);
#endif    

    // This can be either PAROVER or SYNTAX:
    // http://moodle.socs.uoguelph.ca/mod/forum/discuss.php?d=3206#p8234
    START_NEG_TEST( ++negCtr, "Test 2.1 style WORK (PAROVER or SYNTAX)" )                    

    try {                                               
        error = parseVcProp ("ADR;WORK:return to sender", &prop);           

        if ( error != SYNTAX && error != PAROVER ) {                             
            LOG ("Invalid error return %s. Expected %s or %s\n",  
                 VCAM_VcError_toString (error),                  
                 VCAM_VcError_toString (SYNTAX),
                 VCAM_VcError_toString (PAROVER));                 
            FAIL_NEG_TEST;                                  
        }                                                   
    } catch ( exception ) {                             
        LOG ("%s\n", Exception_Names[exception]);       
        FAIL_NEG_TEST;                                  
    }                                                   
    PASS_NEG_TEST;

    NEG_ERROR_TEST("Test SYNTAX: plain line", "Supercalifragilisticexpialidocious", SYNTAX);


    END_SUITE
}


