#include <stdio.h>
#include <miscellany/addr_convert.h>

void addr_to_buffer(void *addr,uint8_t *buffer)
{
	unsigned long data = (unsigned long)addr;
	int i;

	for(i = 0; i < sizeof(int *); i++){
		buffer[i] = data >> 8 * (sizeof(int *) - 1 - i);
	}
}

void *buffer_to_addr(uint8_t *buffer)
{
	void *ret = NULL;
	unsigned long d = 0, t = 0;
	int i;

	for ( i = 0; i < sizeof(int *); i++){
		t = buffer[i];
		d |= t << 8 * (sizeof(int *) - 1 - i); 
	}

	ret = (void *)d;

	return ret;
}

