#include <dirent.h> 
#include <stdio.h> 


int main(void)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir("/home/pi/abkInstant/media/");
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
	if (dir->d_type==DT_DIR)  {
		printf("DIR\t:%s\n", dir->d_name);
	} else if (dir->d_type==DT_REG) {
		printf("FILE\t:%s\n", dir->d_name);
	}
    }

    closedir(d);
  }

  return(0);
}

