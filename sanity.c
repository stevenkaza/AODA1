#include <stdio.h>
#include <stdlib.h>
#include "vcutil.h"
int main()
{

	FILE * fp = fopen("vSample","r"); 
	if (fp==NULL)
	    return 1; 
	
	
         VcFile * vcf = NULL;
	 vcf = malloc(sizeof(VcFile)); 
	 
	 readVcFile(fp,vcf);



}
