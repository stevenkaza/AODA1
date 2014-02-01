// util.h for A1 automarker

#include "vcutil.h"

// This was for W11, when the vcutil spec changed slightly from A1 to A2
// For W14, those A2 changes are now in A1, so we don't define A1MODE
//#define A1MODE 
#define TOLERANCE 0.0001

void * checkMalloc( size_t s );
FILE * fopen_check( char * fname );
const char * VCAM_VcError_toString( VcError e );
char* VCAM_VcProp_toString (VcProp prop);
int string_equal (char* str1, char* str2);
int VcProp_equal (VcProp expected, VcProp actual);
int VcProp_equal_simple (VcProp expected, VcProp actual);
int VcProp_equal_name (VcProp expected, VcProp actual);
int string_heuristic_equal (int photolen, char *start, char *end, char *line);
char* string_toQuotableString (char* str);
int VCAM_Vcard_print (Vcard* card);
int VCAM_VcFile_print (VcFile* vcf);
