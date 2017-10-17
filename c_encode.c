/*charset:utf8*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

/*base64编码-begin:*/
/*base64编码表*/
const char *base64_char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*base64单元编码，将3个byte的原始数据编码为4个byte的base64编码数据*/
void base64_unit_encode(const unsigned char* raw_data,unsigned char* enc_data)
{
    int i;
    unsigned char code;
    unsigned char tmp1,tmp2;
    
    for(i=0;i<4;i++)
    {
        switch(i)
        {
            case 0:
                code = raw_data[0]>>2;
                break;
            case 1:
                tmp1 = raw_data[0]<<6;
                tmp1 = tmp1>>2;
                tmp2 = raw_data[1]>>4;
                code = tmp1|tmp2;
                break;
            case 2:
                tmp1 = raw_data[1]<<4;
                tmp1 = tmp1>>2;
                tmp2 = raw_data[2]>>6;
                code = tmp1|tmp2;
                break;
            case 3:
                tmp1 = raw_data[2]<<2;
                tmp1 = tmp1>>2;
                code = tmp1;
                break;
        }
        enc_data[i] = base64_char[code];
    }
}

/*base64单元解码，将4个byte的原始数据解码为3个byte的base64编码数据*/
int base64_unit_decode(const unsigned char* enc_data,unsigned char* raw_data)
{
    int i,j;
    int unit_len = 3;
    unsigned char enc_code[4];
    unsigned char tmp1,tmp2;
    /*字符转换成base64码值*/
    for(i=0;i<4;i++)
    {
        if(enc_data[i]=='=')
        {
            enc_code[i]=0x0;
            unit_len--;
            continue;
        }
        for(j=0;j<64;j++)
        {
            if(enc_data[i]==base64_char[j])
            {
                enc_code[i]=j;
                break;
            }
                
        }
    }
    /*解码*/
    for(i=0;i<3;i++)
    {
        switch(i)
        {
            case 0:
                tmp1 = enc_code[0]<<2;
                tmp2 = enc_code[1]>>4;
                break;
            case 1:
                tmp1 = enc_code[1]<<4;
                tmp2 = enc_code[2]>>2;
                break;
            case 2:
                tmp1 = enc_code[2]<<6;
                tmp2 = enc_code[3];
                break;
        }
        raw_data[i]=tmp1|tmp2;
    }
    /*处理不足3个字节的部分*/
    if(unit_len!=3)
        {
            switch(unit_len)
            {
                case 1:
                    raw_data[1] = '\0';
                    break;
                case 2:
                    raw_data[2] = '\0';
                    break;
            }
        }
    return unit_len;
}

/*字符串编码*/
int base64_encode(const unsigned char* raw_data,int sum,unsigned char** result)
{
    int i;
    int left_part_len = sum%3;
    int main_unit_len =	sum/3;
    int enc_data_len = main_unit_len*4;
    int byte_pointer_raw,byte_pointer_enc;
    unsigned char left_part[3]={0x0,0x0,0x0};
    unsigned char* enc_data = (unsigned char*)malloc((main_unit_len+1)*4+1);
    
    for(i=0;i<main_unit_len;i++)
    {
        byte_pointer_raw = i*3;
        byte_pointer_enc = i*4;
        base64_unit_encode(raw_data+byte_pointer_raw,enc_data+byte_pointer_enc);
    }
    if(left_part_len!=0)
    {
        byte_pointer_raw = i*3;
        byte_pointer_enc = i*4;
        switch(left_part_len)
        {
            case 1:
                left_part[0] = raw_data[byte_pointer_raw];
                base64_unit_encode(left_part,enc_data+byte_pointer_enc);
                enc_data[byte_pointer_enc+2] = '=';
                enc_data[byte_pointer_enc+3] = '=';
                enc_data_len = byte_pointer_enc+4;
                break;
            case 2:
                left_part[0] = raw_data[byte_pointer_raw];
                left_part[1] = raw_data[byte_pointer_raw+1];
                base64_unit_encode(left_part,enc_data+byte_pointer_enc);
                enc_data[byte_pointer_enc+3] = '=';
                enc_data_len = byte_pointer_enc+4;
                break;
        }
    }
    enc_data[enc_data_len] = '\0';
    *result = enc_data;
    return enc_data_len;
}

/*字符串解码*/
int base64_decode(const unsigned char* enc_data,int sum,unsigned char** result)
{
    int i;
    int unit_len = sum/4;
    int space = unit_len*3;
    int raw_data_len = 0;
    int byte_pointer_raw,byte_pointer_enc;
    unsigned char* raw_data = (unsigned char*)malloc(space);
    for(i=0;i<unit_len;i++)
    {
        byte_pointer_raw = i*3;
        byte_pointer_enc = i*4;
        raw_data_len += base64_unit_decode(enc_data+byte_pointer_enc,raw_data+byte_pointer_raw);
    }
    *result = raw_data;
    return raw_data_len;
}

/*文件编码*/
void base64_file_encode(const char* origin_filepath,const char* encode_filepath)
{
    int raw_len,enc_len;
    FILE* fp1 = fopen(origin_filepath,"rb");
    FILE* fp2 = fopen(encode_filepath,"wb");
    unsigned char raw[3000];
    unsigned char* enc;
    while(1)
    {
        raw_len = fread(raw,1,3000,fp1);
        if(!raw_len)
            break;
        enc_len = base64_encode(raw,raw_len,&enc);
        fwrite(enc,1,enc_len,fp2);
        free(enc);
    }
    fclose(fp1);
    fclose(fp2);
}

/*文件解码*/
void base64_file_decode(const char* encode_filepath,const char* origin_filepath)
{
    int raw_len,enc_len;
    FILE* fp1 = fopen(origin_filepath,"wb");
    FILE* fp2 = fopen(encode_filepath,"rb");
    unsigned char* raw;
    unsigned char enc[4000];
    while(1)
    {
        enc_len = fread(enc,1,4000,fp2);
        if(!enc_len)
            break;
        raw_len = base64_decode(enc,enc_len,&raw);
        fwrite(raw,1,raw_len,fp1);
        free(raw);
    }
    fclose(fp1);
    fclose(fp2);
}
/*base64编码-end*/

/*置换密码编码-begin*/
int permutation_encode(const char* plain_text,int len,const char* key,int key_len,char** result)
{
    int p_cipher = 0;
    int p_key = 0;
    int p_plain = key[p_key]-'1';
    int space;
    int stuff_len = len%key_len;
    char* cipher_text;
    if(stuff_len==0)
        space = len;
    else
        space = (len/key_len+1)*key_len;
    cipher_text = (char*)malloc(space+1);
    for(;p_cipher<space;p_cipher++)
    {
        if(p_plain<len)/*正常读取*/ 
            cipher_text[p_cipher] = plain_text[p_plain];
        else if(p_plain>=space)/*该列已经读取完毕*/ 
        {
            p_key++;
            p_plain = key[p_key]-'1';
            cipher_text[p_cipher] = plain_text[p_plain];
        }
        else/*该列最后一行没有字符*/ 
        {
            p_key++;
            p_plain = key[p_key]-'1';
            cipher_text[p_cipher] = '@';
            continue;	
        }
        /*取该列下一个字符*/
        p_plain = p_plain + key_len;
    }
    cipher_text[space] = '\0';
    *result = cipher_text;
    return space;
}

int permutation_decode(char* cipher_text,int len,char* key,int key_len,char** result)
{
    int p_cipher = 0;
    int p_key = 0;
    int p_plain = key[p_key]-'1';
    int plain_text_len = len;
    char* plain_text;

    plain_text = (char*)malloc(len);
    for(;p_cipher<len;p_cipher++)
    {
        
        if(p_plain>=len)/*该列已经读取完毕*/ 
        {
            p_key++;
            p_plain = key[p_key]-'1';
            plain_text[p_plain] = cipher_text[p_cipher];
        }
        else/*正常读取*/ 
        {
            if(cipher_text[p_cipher] == '@')
                plain_text_len--;
            plain_text[p_plain] = cipher_text[p_cipher];
        }
            
        p_plain = p_plain + key_len;
    }
    plain_text[plain_text_len] = '\0';
    *result = plain_text;
    return plain_text_len;
 } 
/*置换密码-end*/

/*des加密算法begin*/
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
            l |= (l & M32_29)>>28;
            r |= (r & M32_29)>>28;
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

    /*初始置换*/
    for(i=0;i<32;i++)
    {
        tmp64 = (raw << (ip[i] - 1)) & M64_64;
        l |= tmp64 >> (32 + i);
        tmp64 = (raw << (ip[i+32] - 1)) & M64_64;
        r |= tmp64 >> (32 + i);
    }
    
    /*轮加密*/
    for(i=0;i<16;i++)
    {
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
    FILE* fp = fopen(keyfile_path,"wb");
    
    if(fp == NULL)
        return -1;
    
    uint64_t key = des_get_key();
    
    printf("%llx\n",key);
    
    uint8_t key_byte[8];
    uint8_t* key_base64;
    memcpy(key_byte,&key,8);
    len = base64_encode(key_byte,8,&key_base64);
    fwrite(key_base64,1,len,fp);
    fclose(fp);
    return 1;
}

int des_read_keyfile(const char* keyfile_path, uint64_t* key)
{/*读取密钥文件*/
    int len;
    FILE* fp = fopen(keyfile_path,"rb");
    uint8_t key_base64[12];
    uint8_t* key_byte;

    if(fp == NULL)
        return -1;
        
    len = fread(key_base64,1,12,fp);
    
    if(len != 12)
        return -1;
        
    base64_decode(key_base64,12,&key_byte);
    memcpy(key,key_byte,8);
    return 1;
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
        return -1;
        
    des_get_sub_key(key,sub_key);
    while(1)
    {
        len = fread(raw,8,10,origin);
        if(!len)
            break;
            
        for(i=0;i<10;i++)
            des_unit_code(raw[i],sub_key,&processed[i],mode);

        if(fwrite(processed,8,len,des) != len)
            return -1;
    }
    fclose(origin);
    fclose(des);
    return 1;
}

/*des加密算法end*/
