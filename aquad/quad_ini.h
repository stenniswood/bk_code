
#ifdef __cplusplus
extern "C" {
#endif

void show_file( char* mFileName );

void read_ini ( char* mFileName );
void save_ini ( char* mFileName, unsigned char instance1, unsigned char instance2, 
			    float x_alpha, float y_alpha );

#ifdef __cplusplus
}
#endif