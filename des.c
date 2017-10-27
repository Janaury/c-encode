/*des加密算法*/
#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<time.h>
#include"c_encode.h"

//#define DEBUG     //启用以显示调试信息

/*生成子密钥用到的置换*/
int shift[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

int kp1[] = 
{
    57, 49,  41, 33,  25,  17,  9,
     1, 58,  50, 42,  34,  26, 18,
    10,  2,  59, 51,  43,  35, 27,
    19, 11,   3, 60,  52,  44, 36,
    63, 55,  47, 39,  31,  23, 15,
     7, 62,  54, 46,  38,  30, 22,
    14,  6,  61, 53,  45,  37, 29,
    21, 13,   5, 28,  20,  12,  4
};

int kp2[] =   
{   
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

/*f函数中用到的置换和代换*/
uint32_t s[][64] = 
{  
    /*S1*/
    {
        14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
         0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
         4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
        15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
    },
    /*S2*/
    {
        15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
         3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
         0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
        13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
    },
    /*S3*/
    {
        10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
        13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
        13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
         1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
    },
    /*S4*/
    { 
         7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
        13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
        10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
         3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
    },
    /*S5*/
    { 
         2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
        14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
         4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
        11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
    },
    /*S6*/
    {
        12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
        10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
         9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
         4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
    },
    /*S7*/
    { 
         4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
        13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
         1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
         6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
    },
    /*S8*/
    {
        13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
         1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
         7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
         2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
    }
};

int e[] =  
{
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

int p[] = 
{
    16,  7, 20, 21,
    29, 12, 28, 17,
     1, 15, 23, 26,
     5, 18, 31, 10,
     2,  8, 24, 14,
    32, 27,  3,  9,
    19, 13, 30,  6,
    22, 11,  4, 25
};

/*初始置换和其逆置换*/
int ip[] =	
{
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

int iip[] =  
{
    40,  8, 48, 16, 56, 24, 64, 32,
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25
};

/*给密钥添加校验，默认是奇校验*/
void des_add_check(uint8_t* key_byte)
{
  int i;
  uint8_t counter=0,tmp = *key_byte;
  for(i=7;i>0;i--)
    counter += (tmp >> i) & M8_1;
  /*if counter is odd*/ 
  if(counter & M8_1)
    *key_byte = *key_byte & 0xfe; //最后一位置0
  else
    *key_byte = *key_byte | 0x01; //最后一位置1
}

void des_permutation(uint8_t* raw,uint8_t* processed,int processed_bit_len,int* trans_list)
{/*置换选择,内部函数*/
	int i;
    int byte_index,bit_index;
    uint8_t tmp_bit;
    int byte_len = (processed_bit_len + processed_bit_len % 8) / 8;
    memset(processed,0,byte_len);

    for(i=0;i<processed_bit_len;i++)
    {
        byte_index = (trans_list[i] - 1) / 8;
        bit_index = (trans_list[i] - 1) % 8;
        tmp_bit = (raw[byte_index] << bit_index) & M8_8;
        processed[i / 8] |= tmp_bit >> (i % 8);
    }
}

void des_lshift(uint8_t* data,int shift_len)
{/*循环左移*/
    int i;
    uint8_t shift_buff[4];
    memcpy(shift_buff,data,4);
    for(i=0;i<3;i++)
        data[i] = (data[i] << shift_len) | (shift_buff[i+1] >> (8 - shift_len));

    data[3] = (data[3] << shift_len) | ((shift_buff[0]) >> (4 - shift_len)) & M8_8_5;
}

void des_get_sbox_index(uint8_t* data,int index,int* row,int* col)
{/*获取s盒子行号和列号，内部函数*/
    uint8_t data_6bit;
    uint8_t r,c;
    int byte_index = index / 4 * 3;
    switch(index%4)
    {
        case 0:
            data_6bit = (data[byte_index] & M8_8_3) >> 2;
            break;
        case 1:
            data_6bit = ((data[byte_index] & M8_2_1) << 4) | (data[byte_index + 1] >> 4);
            break;
        case 2:
            data_6bit = ((data[byte_index + 1] & M8_4_1) << 2) | (data[byte_index + 2] >> 6);
            break; 
        case 3:
            data_6bit = data[byte_index + 2] & M8_6_1;
            break;
    }
    r = ((data_6bit & M8_6) >> 4) | (data_6bit & M8_1);
    c = (data_6bit & M8_5_2) >> 1;
    *row = r;
    *col = c;
}

void byte_xor(uint8_t* op1,uint8_t* op2,int byte_len,uint8_t* result)
{/*数组异或，内部函数*/
    int i;
    for(i=0;i<byte_len;i++)
        result[i] = op1[i] ^ op2[i];
}

void des_generate_key(uint8_t* key_buff_address)
{/*生成一个密钥*/
  int i;
  uint8_t* key = key_buff_address;
  srand(time(NULL));
  
  for(i=0;i<=7;i++)
  {
    key[i] = rand() % 255;
    /*添加校验，可以去掉*/
    des_add_check(&key[i]);
  }
}

void des_generate_subkey(uint8_t* main_key, Subkey* subkey_buff_address)
{/*生成子密钥*/
    int i,j;
    Subkey* subkey = subkey_buff_address;
    uint8_t l[4],r[4];
    uint8_t tmp_bit;
    uint8_t* origin_array;
    int byte_index,bit_index;
   
    memset(subkey_buff_address,0,16*sizeof(Subkey));

    des_permutation(main_key,l,28,kp1);
    des_permutation(main_key,r,28,kp1+28);
	
    for(i=0;i<16;i++)
    {
    	#ifdef	DEBUG
    	print_byte(l,4,"l_key:");
		print_byte(r,4,"r_key:"); 
		#endif
		
        des_lshift(l,shift[i]);
        des_lshift(r,shift[i]);
        for(j=0;j<48;j++)
        {
            byte_index = ((kp2[j] - 1) % 28) / 8;
            bit_index = ((kp2[j] - 1) % 28) % 8;
            if(kp2[j] <= 28)
                origin_array = l;
            else
                origin_array = r;
            tmp_bit = (origin_array[byte_index] << bit_index) & M8_8;
            subkey[i].byte[j / 8] |= tmp_bit >> (j % 8);
        }

        #ifdef DEBUG
        print_byte(subkey[i].byte,6,"sub_key:");
        #endif

    }
}

void des_unit_process(uint8_t* raw,uint8_t* processed,Subkey* subkey,int mode)
{/*组加密*/
    int i,j;
    int s_row,s_col;
    int key_index;
    uint8_t iip_buff[8];
    uint8_t l[4],r[4];
    uint8_t l_next[4],r_next[4];
    uint8_t data_expand_buff[6];
    uint8_t after_sbox_buff[4];

    des_permutation(raw,l,32,ip);
    des_permutation(raw,r,32,ip+32);
	
    for(i=0;i<16;i++)
    {
    	#ifdef DEBUG
    	printf("index:%d\n",i+1);
    	#endif
    	
        key_index = (mode == 1)? i:15 - i;
        memcpy(l_next,r,4);
        des_permutation(r,data_expand_buff,48,e);
        
        #ifdef DEBUG
        print_byte(l,4,"l:");
        print_byte(r,4,"r:");
        print_byte(subkey[key_index].byte,6,"subkey:");
        print_byte(data_expand_buff,6,"data_expand1:");
        #endif
		
		byte_xor(data_expand_buff,subkey[key_index].byte,6,data_expand_buff);
        
        memset(after_sbox_buff,0,4);
        for(j=0;j<4;j++)
        {
            des_get_sbox_index(data_expand_buff,j << 1,&s_row,&s_col);
            after_sbox_buff[j] |= s[ j << 1 ][s_row * 16 + s_col] << 4;
            des_get_sbox_index(data_expand_buff,(j << 1) + 1,&s_row,&s_col);
            after_sbox_buff[j] |= s[(j << 1) + 1][s_row * 16 + s_col];
        }
		
		//print_byte(subkey[key_index].byte,6,"subkey:");
		
		des_permutation(after_sbox_buff,r_next,32,p);
		
		#ifdef DEBUG
		print_byte(r_next,4,"after_sbox_buff:");
		#endif
        
		byte_xor(l,r_next,4,r_next);
        
		memcpy(l,l_next,4);
        memcpy(r,r_next,4);
    }

    memcpy(iip_buff,r,4);
    memcpy(iip_buff+4,l,4);
    des_permutation(iip_buff,processed,64,iip);
}

int des_generate_keyfile(const char* keyfile_path)
{
	int key_base64_byte_len; 
    uint8_t key[8];
    uint8_t* key_base64;
    FILE* fp;

    fp = fopen(keyfile_path,"wb");
	if(fp == NULL)
		return ERROR;
	
	des_generate_key(key);
	key_base64_byte_len = base64_encode(key,8,&key_base64);
    fwrite(key_base64,1,key_base64_byte_len,fp);
    free(key_base64);
	fclose(fp);
	return OK;
}

int des_read_keyfile(const char* keyfile_path, uint8_t* key)
{
	int len;
	FILE* fp;
	uint8_t key_base64[12];
	uint8_t* key_buff;

    fp = fopen(keyfile_path,"rb");
	if(fp == NULL)
		return ERROR;
		
    if(fread(key_base64,1,12,fp) != 12) return ERROR;
    base64_decode(key_base64,12,&key_buff);
    memcpy(key,key_buff,8);
    free(key_buff);
	return OK;
 } 

int des_file_encode(const char* origin_path,const char* des_path, uint8_t* key)
{/*加密文件，使用密文挪用技术解决短块，请不要加密小于8字节的文件*/
    int i;
	int byte_len,byte_len_next;
	uint8_t raw[8],raw_next[8],processed[8];
	FILE* origin;
	FILE* des;
	Subkey subkey[16];
    
    origin = fopen(origin_path,"rb");
    des = fopen(des_path,"wb");
	if(origin == NULL||des == NULL)
		return ERROR;

    des_generate_subkey(key,subkey);
    byte_len = fread(raw,1,8,origin);
    if(byte_len < 8)
        return ERROR;
        
	while(1)
	{/*预读下一个块，在根据情况处理当前块*/
		byte_len_next = fread(raw_next,1,8,origin);
		if(byte_len_next < 8)
		{/*最后两块处理部分*/
            if(byte_len_next > 0)
            {
                des_unit_process(raw,processed,subkey,1);
                memcpy(raw_next+byte_len_next,processed+byte_len_next,8-byte_len_next);
                
                #ifdef DEBUG
                print_byte(processed,byte_len_next,"last_two_block1:");
                #endif
                
                if(fwrite(processed,1,byte_len_next,des) != byte_len_next) return ERROR;
                des_unit_process(raw_next,processed,subkey,1);
                
                #ifdef DEBUG
                print_byte(processed,8,"last_two_block2:");
                #endif
                
                if(fwrite(processed,1,8,des) != 8) return ERROR;;
            }
            else
            {
                des_unit_process(raw,processed,subkey,1);
                if(fwrite(processed,1,8,des) != 8) return ERROR; 
            }
            break; 
		}
		des_unit_process(raw,processed,subkey,1);
        if(fwrite(processed,1,8,des) != 8) return ERROR;
        /*切换到一下块*/
        byte_len = byte_len_next;
        memcpy(raw,raw_next,8);
	}
	fclose(origin);
	fclose(des);
	return OK;
}

int des_file_decode(const char* origin_path,const char* des_path,uint8_t* key)
{/*解密文件，使用密文挪用技术解决短块，请不要解密小于8字节的文件*/
    int i;
    int write_byte_len;
	int byte_len,byte_len_next;
	uint8_t raw[8],raw_next[8],processed[8],tmp[16];
	FILE* origin;
	FILE* des;
	Subkey subkey[16];
    
    origin = fopen(origin_path,"rb");
    des = fopen(des_path,"wb");
	if(origin == NULL||des == NULL)
		return ERROR;

    des_generate_subkey(key,subkey);
    byte_len = fread(raw,1,8,origin);
    if(byte_len < 8)
        return ERROR;
        
	while(1)
	{
		byte_len_next = fread(raw_next,1,8,origin);
		if(byte_len_next < 8)
		{/*最后两块处理部分*/
            if(byte_len_next > 0)
            {
            	memcpy(tmp,raw,8);
            	memcpy(tmp+8,raw_next,byte_len_next); 
            	
            	#ifdef DEBUG
            	print_byte(tmp,8+byte_len_next,"last two block:");
            	#endif
            	
            	memcpy(raw_next,tmp + byte_len_next,8);
                des_unit_process(raw_next,processed,subkey,0);
                memcpy(raw + byte_len_next,processed + byte_len_next,8-byte_len_next);
                memcpy(tmp,processed,byte_len_next);
                des_unit_process(raw,processed,subkey,0);
                if(fwrite(processed,1,8,des) != 8) return ERROR;
                if(fwrite(tmp,1,byte_len_next,des) != byte_len_next) return ERROR;
            }
            else
            {
                des_unit_process(raw,processed,subkey,0);
                if(fwrite(processed,1,8,des) != 8) return ERROR; 
            }
            break; 
		}
		des_unit_process(raw,processed,subkey,0);
		if(fwrite(processed,1,8,des) != 8) return ERROR;
        
        byte_len = byte_len_next;
        memcpy(raw,raw_next,8);
	}
	fclose(origin);
	fclose(des);
	return OK;
}


