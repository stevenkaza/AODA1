/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#include <string.h>
#include "exception.h"
#include "mark.h"
#include "vcutil.h"
#include "util.h"

#define NUM_POS_TESTS 1
#define NUM_NEG_TESTS 1

int s5( int suite, int weight, int argc, char *argv[] ) {
//    int posCtr = 0;	(W14) now in mark.h
    int exception;

    VcProp prop = { VCP_N, NULL, NULL, NULL, NULL };

    prop.partype = malloc (sizeof (char*)*5);
    strncpy (prop.partype, "TEST", 5);

    prop.parval = malloc (sizeof (char*)*5);
    strncpy (prop.parval, "ESTE", 5);

    prop.value = malloc (sizeof (char*)*5);
    strncpy (prop.value, "STET", 5);

    Vcard* card = malloc (sizeof (Vcard) + sizeof (VcProp));
    card->nprops = 1;
    (card->prop)[0] = prop;

    VcFile vcf = { 1, NULL };
    vcf.cardp = malloc (sizeof (Vcard*));
    *(vcf.cardp) = card;
    


    START_SUITE( suite, "freeVcFile", weight, NUM_POS_TESTS, NUM_NEG_TESTS );
    
    START_POS_TEST ( ++posCtr, "Test freeVcFile");

    try {
        freeVcFile (&vcf);
    } catch (exception) {
        LOG ("%s\n", Exception_Names[exception]);
        FAIL_POS_TEST;
    }
    PASS_POS_TEST;


//    int negCtr = 0;	(W14) now in mark.h

    START_NEG_TEST ( ++negCtr, "Test that at least something was freed");

    try {
        // not a perfect test yet, but at least we'll get an idea
        // TODO: in the future, compare the memory used before allocated, after allocation, and after freeing instead
        free ((vcf.cardp)[0]->prop[0].value);
        free ((vcf.cardp)[0]);
        free (vcf.cardp);
        printf ("We were able to manually free all the allocated data you should have freed, unfortunately.\n");
        FAIL_NEG_TEST;
    } catch (exception) {
        // yay, freeVcFile actually freed something :)  may not have freed all though :(
    }
    PASS_NEG_TEST;



    END_SUITE
}
