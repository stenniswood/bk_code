


//#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
//#define Dprintf if (Debug) printf("%s ", __FILENAME__); if (Debug) printf


#ifndef radians
#define radians(arg) ((arg)*3.1415/180.0)
#define degrees(arg) ((arg)*180.0/3.1415)
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif 
