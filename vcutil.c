#include "vcutil.h"
/*Pre assumption:
	Assume vcf and filep are not NULL */ 
VcStatus readVcFile (FILE *const vcf, VcFile *const filep)
{
 	filep->ncards = 0; 
	filep->cardp = NULL;
	
	/*Now we have to figure out what we do with each vcard and the flow of the program */ 

}



VcStatus readVcard( FILE * const vcf, Vcard **const cardp)
{

}

VcStatus getUnfolded ( FILE * const vcf, char **const buff ) 
{

} 

VcError parseVcProp ( const char * buff, VcProp * const propp)
{
} 

void freeVcFile ( VcFile * const filep) 
{
} 




