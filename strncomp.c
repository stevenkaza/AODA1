#include <stdio.h>

#include <string.h>
int main ()
{
	char string[100];
	strcpy(string,"BEGIN:VCARD\r\n");
	char lineBuffer[100];

	FILE * fp = NULL;
	fp= fopen("samples-10.vcf","r");
	fgets(lineBuffer,100,fp);
	printf("%s\n",lineBuffer);
	printf("%d\n",strncmp(string,lineBuffer,strlen(lineBuffer)));
	return 0; 




}