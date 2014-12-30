#include <stdlib.h>





void init_power_off		() 
{ 
	exit(1);
}	

void init_power_restart  () 
{ 
	char cmd[] = "sudo shutdown -r now";
	
}	

void init_power_sleep	() 
{ 
}	

void init_logoff_user	() 
{
	exit(1); 
}	


