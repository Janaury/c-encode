/*置换密码编码*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"c_encode.h"

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
