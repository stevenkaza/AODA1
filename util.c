/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

// util.c for A1 automarker

#define _GNU_SOURCE // for asprintf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exception.h"
#include "mark.h"
#include "util.h"

void * checkMalloc( size_t s ) {
    void * ret = malloc( s );

    if ( ret == NULL ) {
        LOG( "AUTOMARKER MALLOC FAILED\n" );
        abort();
    }

    return ret;
}

FILE * fopen_check( char * fname ) {
    FILE * fp = fopen( fname, "r" );

    if ( fp == NULL ) {
        LOG( "AUTOMARKER FATAL Could not open file '%s'\n", fname );
        abort();
    }

    return fp;
}

const char * VCAM_VcError_toString( VcError e ) {
    switch ( e ) {
    case OK:
        return "OK";
    case IOERR:
        return "IOERR";
    case SYNTAX:
        return "SYNTAX";
    case PAROVER:
        return "PAROVER";
    case BEGEND:
        return "BEGEND";
    case BADVER:
        return "BADVER";
    case NOPVER:
        return "NOPVER";
    case NOPNFN:
        return "NOPNFN";
    default:
        return "UNKNOWN VCERROR";
    }
}

char* VCAM_VcPname_strs[] = {
    "VCP_BEGIN",
    "VCP_END",
    "VCP_VERSION",
    "VCP_N",      // name
    "VCP_FN",     // formatted name
    "VCP_NICKNAME",
    "VCP_PHOTO",
    "VCP_BDAY",
    "VCP_ADR",    // address
    "VCP_LABEL",  // address formatted for printing
    "VCP_TEL",
    "VCP_EMAIL",
    "VCP_GEO",    // lat,long
    "VCP_TITLE",
    "VCP_ORG",
    "VCP_NOTE",
    "VCP_UID",    // unique ID
    "VCP_URL",
    "VCP_OTHER"   // used for any other property name
};

// must free result
char* string_toQuotableString (char* str) {
    char *qstr;
    if (str != NULL) {
        asprintf (&qstr, "\"%s\"", str);
    } else {
        asprintf (&qstr, "NULL");
    }
    return qstr;    
}

// must free result
char* VCAM_VcProp_toString (VcProp prop) {
    char *str;
    char *partype = string_toQuotableString (prop.partype);
    char *parval = string_toQuotableString (prop.parval);
    char *value = string_toQuotableString (prop.value);
    asprintf (&str, "{ %s, %s, %s, %s, %s }", // requires #define _GNU_SOURCE before #include <stdio.h>
              VCAM_VcPname_strs[prop.name], partype, parval, value, prop.hook == NULL ? "NULL" : "<NOTNULL>");
    free (partype);
    free (parval);
    free (value);

    return str;
}

// this doesn't do a complete comparison, just checks the start and end for equivalence
int string_heuristic_equal (int photolen, char *start, char *end, char *line) {
    // What I'm doing might be too fancy, but at least there's try {} catch :D 

    int portion_min_len = strlen(start)+3+strlen(end);
    char line_portion[portion_min_len + 1];
    
    if (line == NULL) {
        LOG ("Line NULL did not match expected '%s...%s' (%d chars).\n",
             start, end, photolen);

        return 0;
    } else {
        if (strlen (line) > portion_min_len) {
            strncpy (line_portion, line, strlen(start));
            strncpy (&(line_portion[strlen(start)]), "...", 3);
            strncpy (&(line_portion[strlen(start)+3]), &(line[strlen(line)-strlen(end)]), strlen(end));
            line_portion[portion_min_len] = '\0';
        } else {
            strncpy (line_portion, line, portion_min_len);
            line_portion[strlen(line)] = '\0';
        }
        
        if (strlen (line) != photolen
            || 0 != strncmp (line, start, strlen(start)) 
            || 0 != strncmp (&(line[photolen-strlen(end)]), end, strlen(end))) {  // 24 (photolen==strlen(line) now)
            LOG ("Line '%s' (%d chars) did not match expected '%s...%s' (%d chars).\n",
                 line_portion, (int)strlen(line), start, end, (int)photolen);
            return 0;
        }
    }

    return 1;
    
}

// returns 1 for equal, 0 for not 
int string_equal (char* str1, char* str2) {
    if (str1 == str2)
        return 1;
    if (str1 == NULL || str2 == NULL)
        return 0;
    if (strlen (str1) != strlen (str2))
        return 0;
    if (strncmp (str1, str2, strlen (str1)) != 0)
        return 0;
    return 1;
}

int VcProp_equal (VcProp expected, VcProp actual) {

#ifdef A1MODE
    /* ambiguous specification means that when checking VCP_OTHER we
       should only check name.  In A2, we check it all! */
    if (expected.name == VCP_OTHER && actual.name == VCP_OTHER) {
        return 1;
    }
#endif    

    if (expected.name == actual.name
        && string_equal (expected.partype, actual.partype)
        && string_equal (expected.parval, actual.parval)
        && string_equal (expected.value, actual.value)
        && expected.hook == actual.hook) {
        return 1;
    } else {
        return 0;
    }
}

int VcProp_equal_name (VcProp expected, VcProp actual) {
    if (expected.name == actual.name) {
        return 1;
    } else {
        return 0;
    }
}

int VcProp_equal_simple (VcProp expected, VcProp actual) {
    if (expected.name == actual.name
        && string_equal (expected.value, actual.value)) {
        return 1;
    } else {
        return 0;
    }
}

int VCAM_Vcard_print (Vcard* card) {
  VcProp prop;

  printf ("    card has %d properties\n\n", card->nprops);
  for (int j = 0; j < card->nprops; j++) {
      prop = (card->prop)[j];
      printf ("    %2d. %d/%s:%s\n", j, prop.name, VCAM_VcPname_strs[prop.name], prop.value);
      if (prop.partype != NULL) {
          printf ("        TYPE=%s\n", prop.partype);
      }
      if (prop.parval != NULL) {
          printf ("        VALUE=%s\n", prop.parval);
      }
  }

  return 1;
}

int VCAM_VcFile_print (VcFile* vcf) {
  printf ("vcfile has %d vcards\n", vcf->ncards);

  for (int i = 0; i < vcf->ncards; i++) {
    printf ("  card %d\n", i);

    VCAM_Vcard_print ((vcf->cardp)[i]);
    printf ("\n");    
  }

  return 1;
}
