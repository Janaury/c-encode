/*charset:utf8*/
#include<stdio.h>
#include<stdlib.h>

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
/*base64编码-end:*/

/*置换密码编码-begin:*/
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

/*置换密码-end:*/