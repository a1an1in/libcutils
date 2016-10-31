#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <libdbg/debug.h>

void print_time_stamp()  
{  
    struct timeval    tv;  
    struct timezone   tz;  
    struct tm         *p;  

    gettimeofday(&tv, &tz);  
    p = localtime(&tv.tv_sec);  
    printf("time_now:%d /%d /%d %d :%d :%d.%3ld", 1900+p->tm_year, 1+p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);  
}  

void printf_buffer(unsigned char *buf, int len)
{
#define MAX_PRINT_BUFFER_LEN 1024                   
	char buffer[MAX_PRINT_BUFFER_LEN];                      
	int i;                           
	int buffer_len;

	if(len > MAX_PRINT_BUFFER_LEN){                      
		dbg_str(DBG_ERROR,"buffer too long");               
		return ;                      
	}                            

	memset(buffer,0,MAX_PRINT_BUFFER_LEN);                      
	buffer_len = strlen(buffer);
	
	for(i = 0; i < len; i++) {                   
		snprintf(buffer + buffer_len , MAX_PRINT_BUFFER_LEN,"%c ",buf[i]);               
		buffer_len = strlen(buffer);
	}                           

	snprintf(buffer + buffer_len,MAX_PRINT_BUFFER_LEN, "\n");                      

    printf("%s",buffer);
#undef MAX_PRINT_BUFFER_LEN                    
}
