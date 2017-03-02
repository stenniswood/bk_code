


char sql_username[] 		= "stenniswood";
char sql_password[] 		= "Ever#rdy92";
char sql_user_dbase_name[] 		   = "sjtennis_bk_useraccounts";
char sql_global_knowledge_name[]   = "sjtennis_global_knowledge";
char sql_ad_dbase_name[]   		   = "sjtennis_advertisements";


#if (PLATFORM==linux_desktop)
char shared_mem_ids_base_path[] = "/home/stenniswood/bk_code/shm_ids/";
#elif (PLATFORM==RPI)
char shared_mem_ids_base_path[] = "/home/pi/bk_code/shm_ids/";
#elif (PLATFORM==MAC)
char shared_mem_ids_base_path[] = "/Users/stephentenniswood/code/bk_code/shm_ids/";
#endif



/* What SQL connections are we using?
	sql_people
	adrenaline_protocol

*/