/*des加密算法*/
#include<stdio.h>
#include<string.h>
#include<time.h>
#include"c_encode.h"

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


void des_add_check(uint8_t* key_byte)
{/*给密钥添加校验，默认是奇校验*/
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


uint64_t des_get_key()
{/*生成一个密钥*/
  int i;
  uint8_t tmp8 = 0;
  uint64_t tmp64 = 0;
  uint64_t key = 0;
  srand(time(NULL));
  
  for(i=56;i>=0;i-=8)
  {
    tmp8 = rand() % 255;
    /*添加校验，可以去掉*/
    des_add_check(&tmp8);
    tmp64 = (uint64_t)tmp8;
    key |= tmp64 << i;
  }
  return key;
}

void des_get_sub_key(uint64_t key,uint64_t* sub_key)
{/*生成16个子密钥*/
    int i,j;
    uint32_t l = 0,r = 0;
    uint64_t lr = 0,tmp64 = 0;
  
  /*密钥初始置换*/
    for(i=0;i<28;i++)
    {
        tmp64 = (key << (kp1[i] - 1)) & M64_64;
        l |= tmp64 >> (36+i);
        tmp64 = (key << (kp1[i+28] - 1))& M64_64;
        r |= tmp64 >> (36+i);
    }
    /*循环左移，生成16个子密钥*/
    for(i=0;i<16;i++)
    {
        l = l << shift[i];
        r = r << shift[i];
        if(shift[i]==1)
        {
            l |= (l & M32_29) >> 28;
            r |= (r & M32_29) >> 28;
        }
        else
        {
            l |= (l & M32_30_29)>>28;
            r |= (r & M32_30_29)>>28;
        }

        /*合并左右两部分，准备生成子密钥*/
        lr = l;
        lr = lr << 28;
        lr |= r & 0x0ffffff;
        
        /*生成子密钥*/
        sub_key[i] = 0;
        for(j=0;j<48;j++)
        {
            tmp64 = (lr << (kp2[j]-1)) & M64_56;
            sub_key[i] |= tmp64 >> (8 + j);
        }
    }
}

void des_unit_code(uint64_t raw,uint64_t* sub_key,uint64_t* processed,int mode)
{/*des一个单元(64bit)编码*/
    int i,j;
    int key_index = 0;
    uint8_t tmp8 = 0;
    uint8_t x = 0,y = 0;    //用于s盒子索引
    uint32_t tmp32 = 0;
    uint32_t l = 0,r = 0,l_n = 0,r_n = 0;
    uint32_t after_s = 0;
    uint64_t tmp64 = 0;
    uint64_t before_s = 0;
    uint64_t before_iip = 0;
    uint64_t finish = 0;

    
    for(i=0;i<32;i++)
    {/*初始置换*/
        tmp64 = (raw << (ip[i] - 1)) & M64_64;
        l |= tmp64 >> (32 + i);
        tmp64 = (raw << (ip[i+32] - 1)) & M64_64;
        r |= tmp64 >> (32 + i);
    }
    
    
    for(i=0;i<16;i++)
    {/*轮加密*/

        /*初始化中间变量*/
        l_n = 0;        //下一轮的左部分数据
        r_n = 0;        //  下一轮的右部分数据
        before_s = 0;   //存放32位原始数据扩展后的48位数据
        after_s = 0;    //存放s盒代换后的32位数据
        key_index = (mode == 1)? i:15-i;    //根据加密或解密选择密钥
        
        l_n = r;        //填充下一轮的左部分数据

        /*32bit数据扩展到48bit（选择运算E）*/
        for(j=0;j<48;j++)
        {
            tmp64 = r << (e[j]-1);
            tmp64 = (tmp64 & M64_32) << 16;
            before_s |= tmp64 >> j;
        }
        /*同子密钥异或*/
        before_s ^= sub_key[key_index];
        
        /*s盒运算*/
        for(j=0;j<8;j++)
        {
            tmp8 = (before_s >> (42-j*6)) & M64_6_1;
            y = ((tmp8 & 0x20) >> 4) | (tmp8 & M8_1);
            x = (tmp8 & M8_5_2) >> 2;
            tmp32 = s[j][(y<<4)+x];
            after_s |= tmp32 << (28 - (j<<2));
        }
        /*32bit数据再次置换（置换运算P）*/
        for(j=0;j<32;j++)
        {
            tmp32 = (after_s << (p[j] - 1)) & M32_32;
            r_n |= tmp32 >> j;
        }
        r_n ^= l;
        l = l_n;
        r = r_n;
    }

    /*合并左右两部分，得到中间结果*/
    tmp64 = 0;
    tmp64 = r;
    tmp64 = tmp64 << 32;
    tmp64 |= l;
    before_iip = tmp64;

    /*最后的逆置换*/
    for(i=0;i<64;i++)
    {
        tmp64 = (before_iip<< (iip[i]-1)) & M64_64;
        finish |= tmp64 >> i;
    }
    *processed = finish;
}

int des_generate_keyfile(const char* keyfile_path)
{/*生成密钥文件*/
    int len;
    uint8_t key_byte[8];
    uint8_t* key_base64; 
    FILE* fp = fopen(keyfile_path,"wb");
    
    if(fp == NULL)
        return ERROR;
    
    uint64_t key = des_get_key();
    memcpy(key_byte,&key,8);
    len = base64_encode(key_byte,8,&key_base64);
    fwrite(key_base64,1,len,fp);
    fclose(fp);
    return OK;
}

int des_read_keyfile(const char* keyfile_path, uint64_t* key)
{/*读取密钥文件*/
    int len;
    FILE* fp = fopen(keyfile_path,"rb");
    uint8_t key_base64[12];
    uint8_t* key_byte;

    if(fp == NULL)
        return ERROR;
        
    len = fread(key_base64,1,12,fp);
    
    if(len != 12)
        return ERROR;
        
    base64_decode(key_base64,12,&key_byte);
    memcpy(key,key_byte,8);
    return OK;
 } 

int des_file_code(const char* origin_path,const char* des_path,uint64_t key,int mode)
{/*加密（解密）文件*/
    int len;
    int i;
    FILE* origin = fopen(origin_path,"rb");
    FILE* des = fopen(des_path,"wb");
    uint64_t raw[10],processed[10];
    uint64_t sub_key[16];
    
    if(origin == NULL||des == NULL)
        return ERROR;
        
    des_get_sub_key(key,sub_key);
    while(1)
    {
        len = fread(raw,8,10,origin);
        if(!len)
            break;
            
        for(i=0;i<10;i++)
            des_unit_code(raw[i],sub_key,&processed[i],mode);

        if(fwrite(processed,8,len,des) != len)
            return ERROR;
    }
    fclose(origin);
    fclose(des);
    return OK;
}
