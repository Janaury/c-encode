#include<stdio.h>
#include<stdlib.h>
#include "c_encode.h"

/*测试程序*/
int main()
{
	base64_file_encode("./p.rar","./p.base64");
	return 0;
 } 

