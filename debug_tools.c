#include<stdio.h>
#include "c_encode.h"

void print_byte(uint8_t* buff,int len,char* notice)
{/*以16进制的形式打印从buff开始的len字节内存，notice会作为提示显示在数据前面*/
    int i;
    printf("%s(0x)\n",notice);
    for(i=0;i<len;i++)
	{
		printf("%02x-",buff[i]);
		if((i+1) % 16 == 0 && (i + 1) != len) 
			printf("\n");
	}
	printf("\n");
}