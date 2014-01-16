#include <stdio.h>
#include <string.h>

int main()
{
	FILE * fp; 
	fp = fopen("parse.txt","r");

	char lineBuffer[100];

    while (fgets(lineBuffer,100,fp)!=NULL)
    {
    	if (strstr(lineBuffer,"\r\n")==NULL)
    		printf("%s\n",lineBuffer);
    }
}