#include <stdio.h>
#include <miscellany/buffer.h>
#include <libdbg/debug.h>

void addr_to_buffer(void *addr,uint8_t *buffer)
{
	unsigned long data = (unsigned long)addr;
	int i;

	for(i = 0; i < (int)sizeof(int *); i++){
		buffer[i] = data >> 8 * (sizeof(int *) - 1 - i);
	}
}

void *buffer_to_addr(uint8_t *buffer)
{
	void *ret = NULL;
	unsigned long d = 0, t = 0;
	int i;

	for ( i = 0; i < (int)sizeof(int *); i++){
		t = buffer[i];
		d |= t << 8 * (sizeof(int *) - 1 - i); 
	}

	ret = (void *)d;

	return ret;
}

void *buffer_to_addr_wb(uint8_t *buffer, void **ret)
{
	unsigned long d = 0, t = 0;
	int i;

	for ( i = 0; i < (int)sizeof(int *); i++){
		t = buffer[i];
		d |= t << 8 * (sizeof(int *) - 1 - i); 
	}

	*ret = (void *)d;

	return (void *)d;
}

void printf_buffer(unsigned char *buf, int len)
{
#define MAX_PRINT_BUFFER_LEN 1024 *40                   
	char buffer[MAX_PRINT_BUFFER_LEN];                      
	int i;                           
	int buffer_len;

	if(len * 2 > MAX_PRINT_BUFFER_LEN){                      
		dbg_str(DBG_ERROR,"buffer too long,please check,len=%d",len);               
		return ;                      
	}                            

	memset(buffer,0,MAX_PRINT_BUFFER_LEN);                      
	buffer_len = strlen(buffer);
	
	for(i = 0; i < len; i++) {                   
		snprintf(buffer + buffer_len , MAX_PRINT_BUFFER_LEN,"%x ",buf[i]);               
		buffer_len = strlen(buffer);
	}                           

	snprintf(buffer + buffer_len,MAX_PRINT_BUFFER_LEN, "\n");                      

    printf("%s\n",buffer);
#undef MAX_PRINT_BUFFER_LEN                    
}
