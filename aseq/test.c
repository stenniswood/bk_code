#include <stdlib.h>
#include <stdio.h>

int main()
{
	int tmp=0;
	sscanf("0x108", "%x", &tmp ); 
	printf(" i= %x %d \n", tmp, tmp );
}

