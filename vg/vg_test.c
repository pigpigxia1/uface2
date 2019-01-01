#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>


#define WIEGAND_PATH "/dev/wgread"



int main(int argc , char *argv[])
{
	int fd;
	int n;	
	int ret = 0;
	unsigned char buff[4];
	unsigned char wg_buff[3]; 
	fd_set readfs;
	///struct timeval Timeout;
	
	fd = open(WIEGAND_PATH, O_RDWR | O_NOCTTY |O_NDELAY);
    if (fd < 0) {
        printf("open /dev/wgout failed\n");
        return -1;
    }   
	printf("open device success! fd = %d\n",fd);
	//Timeout.tv_usec = 0;
	//Timeout.tv_sec  = 5;
	
	//FD_ZERO(&readfs);
	//FD_SET(fd, &readfs);
	
	while(1)
	{
		
		printf("read data\n");
		ret = read(fd, buff, 4); 
		if (ret < 0){ 
			printf("read failed\n");
			//return -1;
		}   
		for(n = 0; n < 4; n++)
		{
			printf("%x\t",buff[n]);
		}
		/*ret = select(fd+1, &readfs, NULL, NULL, NULL);
		if(ret == 0)
		{
			printf("time out!\n");
			continue;
		}
		else if(ret < 0)
		{
			printf("select error!\n");
			
		}
		else
		{
			if (FD_ISSET(fd,&readfs))
			{
				
				ret = read( fd, buff, 4);
				printf("read data!ret = %d\n",ret);
				for(n = 0; n < 4; n++)
				{
					printf("%x\t",buff[n]);
				}
			}
		}*/
		
		
		/*ret = read(fd, buff, 4);
		printf("read data!ret = %d\n",ret);*/
		usleep(500000);
		
	}
	
    

    
	
	close(fd);
	return 0;
}