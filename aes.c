#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "c_encode.h"
#include "aes_trans_list.h"

//#define DEBUG     //开启DEBUG可以看到中间结果

/*这两个函数在理解AES算法时可以使用，但其运算消耗很大，很慢，一般不使用，而使用查表的方式来加速
uint8_t mul_gf2_8(uint8_t op1,uint8_t op2)
{//有限域gf2^8上的乘法
    int i;
	uint8_t mod = 0x1b;
	uint8_t result = 0;
	for(i=0;i<8;i++)
	{
		if(op2 & 0x01)
			result ^= op1;
        op2 = op2 >> 1;
        op1 = (op1 & 0x80)? (op1 << 1) ^ mod : op1 << 1;
	}
	 return result; 
}
int _aes_mix_column(uint8_t* data,uint8_t* matrix)
{//直接做矩阵乘法的列混合，依赖gf2^8上的乘法函数
    int i,j;
    uint8_t result_buff[4];
    memset(result_buff,0,4);
    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
            result_buff[i] ^= mul_gf2_8(data[j], matrix[i * 4 + j]);
    
	*(uint32_t*)data = *(uint32_t*)result_buff;
}
*/
uint8_t xtime(uint8_t data)
{
    const uint8_t mod = 0x1b;
    data = (data & M8_8)? (data << 1) ^ mod : data << 1;
    return data;
}

void byte_sub(uint8_t* data, int byte_len, const uint8_t trans_list[])
{
    int i;
    for(i = 0;i < byte_len; i++)
        data[i] = trans_list[data[i]];
}


void aes_shift_row(uint8_t* data, const int shift_len, const int shift_used_for_mul_len)
{//第三个参数是为了解决同一行的字节不连续的问题，同时在key_expansion中能够复用，同时考虑执行效率产生的
    int after_shift_index;
    int r_shift_len = 4 - shift_len;    //把左移编编程右移
    uint8_t shift_buff[8];
    
    //保存原始数据
    shift_buff[0] = data[0 << shift_used_for_mul_len];
    shift_buff[1] = data[1 << shift_used_for_mul_len];
    shift_buff[2] = data[2 << shift_used_for_mul_len];
    shift_buff[3] = data[3 << shift_used_for_mul_len];
    //按字节移位    
    after_shift_index = ((0 + r_shift_len) & M8_2_1) << shift_used_for_mul_len;     //1
    data[after_shift_index] = shift_buff[0];                                        
    after_shift_index = ((1 + r_shift_len) & M8_2_1) << shift_used_for_mul_len;     //2
    data[after_shift_index] = shift_buff[1];                                                
	after_shift_index = ((2 + r_shift_len) & M8_2_1) << shift_used_for_mul_len;     //3
    data[after_shift_index] = shift_buff[2];
    after_shift_index = ((3 + r_shift_len) & M8_2_1) << shift_used_for_mul_len;     //4
    data[after_shift_index] = shift_buff[3];
}

int aes_mix_column(uint8_t* data,const uint32_t trans_list[][256] )
{//将矩阵运算分解后生成表，通过查表加速的列混合
    int i;
    uint32_t tmp = 0;
    tmp = trans_list[0][data[0]];
    tmp ^= trans_list[1][data[1]];
    tmp ^= trans_list[2][data[2]];
    tmp ^= trans_list[3][data[3]];
    *(uint32_t*)data = tmp;
}

void add_round_key(uint8_t* data,uint32_t* round_key)
{//对128bit数据做异或运算
    *(uint64_t*)data ^= *(uint64_t*)round_key;
    *((uint64_t*)data + 1) ^= *((uint64_t*)round_key + 1);
}

void aes_key_expand(uint32_t* key, uint32_t* expanded_key)
{
    int i;
    int expanded_key_word_len = 4 * (4 + 4 + 2 + 1);        
    uint32_t tmp_word;
    uint8_t* tmp_word_byte = (uint8_t*)(&tmp_word);     //用于按字节操作tmp_word中的4个字节
    uint8_t RC = 0x01;
    uint8_t Rcon[4] = {0x0, 0x0, 0x0, 0x0};
    
    memcpy(expanded_key, key, 4 * 4);
    for(i = 4; i < expanded_key_word_len; i++)
    {
        tmp_word = expanded_key[i - 1];
        if(i % 4 == 0 )
        {
            aes_shift_row(tmp_word_byte, 1, 0);
            byte_sub(tmp_word_byte, 4, aes_sbox);
            
            Rcon[0] = RC;
            tmp_word = tmp_word ^ *(uint32_t*)Rcon;
            RC = xtime(RC);
        }
        expanded_key[i] = expanded_key[i - 4] ^ tmp_word;
    }
}

void aes_generate_key(uint32_t* key_buff_address)
{
    int i;
    uint8_t* key = (uint8_t*)key_buff_address;
    srand(time(NULL));
    for(i=0;i<16;i++)
        key[i] = rand() & 0xff;
}

void aes_unit_encode(uint8_t* raw, uint8_t* processed, uint32_t* expanded_key)
{
    int i;
    uint32_t* round_key = expanded_key;
    uint8_t processed_buff[16];
    memcpy(processed_buff, raw, 16);

        #ifdef DEBUG
        print_byte(processed_buff, 16, "raw:");
        #endif

    add_round_key(processed_buff, round_key);

        #ifdef DEBUG
        print_byte(processed_buff, 16, "start round:");
        print_byte((uint8_t*)round_key, 16, "round_key:");
        #endif

    round_key += 4;
    for(i=0;i<9;i++)
    {
        byte_sub(processed_buff, 16, aes_sbox);
        
            #ifdef DEBUG
            printf("index:%d\n",i+1);
            print_byte(processed_buff,16,"sub_byte:");
            #endif
        
        aes_shift_row(processed_buff + 1, 1, 2);
        aes_shift_row(processed_buff + 2, 2, 2);
        aes_shift_row(processed_buff + 3, 3, 2);
        
            #ifdef DEBUG
            print_byte(processed_buff,16 ,"shift_row:");
            #endif
        
        aes_mix_column(processed_buff, aes_mix_column_translist);
        aes_mix_column(processed_buff + 4, aes_mix_column_translist);
        aes_mix_column(processed_buff + 8, aes_mix_column_translist);
        aes_mix_column(processed_buff + 12, aes_mix_column_translist);
        
            #ifdef DEBUG
            print_byte(processed_buff, 16, "mix_column:");
            #endif
        
        add_round_key(processed_buff, round_key);
        
            #ifdef DEBUG
            print_byte((uint8_t*)round_key, 16, "round_key:");
            print_byte(processed_buff, 16, "add_round_key:");
            printf("\n");
            #endif
        
        round_key += 4;
    }
    byte_sub(processed_buff, 16, aes_sbox);
    aes_shift_row(processed_buff + 1, 1, 2);
    aes_shift_row(processed_buff + 2, 2, 2);
    aes_shift_row(processed_buff + 3, 3, 2);
    add_round_key(processed_buff, round_key);
    memcpy(processed,processed_buff,16);
}

void aes_unit_decode(uint8_t* raw,uint8_t* processed,uint32_t* expanded_key)
{//逆序执行加密流程
    int i;
    uint32_t* round_key = expanded_key + 40;
    uint8_t processed_buff[16];
    memcpy(processed_buff, raw, 16);

    add_round_key(processed_buff, round_key);
    aes_shift_row(processed_buff + 1, -1, 2);
    aes_shift_row(processed_buff + 2, -2, 2);
    aes_shift_row(processed_buff + 3, -3, 2);
    byte_sub(processed_buff,16,aes_inv_sbox);
    for(i=0;i<9;i++)
    {
        round_key -= 4;
        add_round_key(processed_buff, round_key);
        
        aes_mix_column(processed_buff, aes_inv_mix_column_translist);
        aes_mix_column(processed_buff + 4, aes_inv_mix_column_translist);
        aes_mix_column(processed_buff + 8, aes_inv_mix_column_translist);
        aes_mix_column(processed_buff + 12, aes_inv_mix_column_translist);
        
        aes_shift_row(processed_buff + 1, -1, 2);
        aes_shift_row(processed_buff + 2, -2, 2);
        aes_shift_row(processed_buff + 3, -3, 2);
        byte_sub(processed_buff, 16, aes_inv_sbox);
    }
    round_key -= 4;
    add_round_key(processed_buff, round_key);
    memcpy(processed, processed_buff, 16); 
}

int aes_generate_keyfile(const char* keyfile_path)
{
	int key_base64_byte_len; 
    uint8_t key[16];
    uint8_t* key_base64;
    FILE* fp;

    fp = fopen(keyfile_path, "wb");
	if(fp == NULL)
		return ERROR;
	
	aes_generate_key((uint32_t*)key);
	key_base64_byte_len = base64_encode(key, 16, &key_base64);
    fwrite(key_base64, 1, key_base64_byte_len, fp);
    free(key_base64);
	fclose(fp);
	return OK;
}

int aes_read_keyfile(const char* keyfile_path, uint32_t* key)
{
	int len;
	FILE* fp;
	uint8_t key_base64[24];
	uint8_t* key_buff;

    fp = fopen(keyfile_path,"rb");
	if(fp == NULL)
		return ERROR;
		
    if(fread(key_base64,1,24,fp) != 24) return ERROR;
    base64_decode(key_base64, 24, &key_buff);
    memcpy(key, key_buff, 16);
    free(key_buff);
	return OK;
} 

int aes_file_encode(const char* origin_path, const char* des_path, uint32_t* key)
{/*加密文件，使用密文挪用技术解决短块，请不要加密小于8字节的文件*/
    int i;
	int byte_len,byte_len_next;
    uint8_t raw[16], raw_next[16], processed[16];
    uint32_t expanded_key[44];
	FILE* origin = fopen(origin_path, "rb");
	FILE* des = fopen(des_path, "wb");
	
    origin = fopen(origin_path,"rb");
    des = fopen(des_path, "wb");
	if(origin == NULL || des == NULL)
		return ERROR;

    aes_key_expand(key, expanded_key);
    
    byte_len = fread(raw, 1, 16, origin);
    if(byte_len < 16)
        return ERROR;
	for(;;) 
	{/*预读下一个块，在根据情况处理当前块*/
		byte_len_next = fread(raw_next, 1, 16, origin);
		if(byte_len_next < 16)
		{/*最后两块处理部分*/
            if(byte_len_next > 0)
            {
                aes_unit_encode(raw, processed, expanded_key);
                memcpy(raw_next + byte_len_next, processed + byte_len_next, 16 - byte_len_next);
                
                    #ifdef DEBUG
                    print_byte(processed, byte_len_next, "last_two_block1:");
                    #endif
                
                if(fwrite(processed, 1, byte_len_next, des) != byte_len_next) return ERROR;
                aes_unit_encode(raw_next, processed, expanded_key);
                
                    #ifdef DEBUG
                    print_byte(processed, 16,"last_two_block2:");
                    #endif
                
                if(fwrite(processed,1,16,des) != 16) return ERROR;;
            }
            else
            {
                aes_unit_encode(raw, processed, expanded_key);
                if(fwrite(processed,1,8,des) != 16) return ERROR; 
            }
            break; 
		}
		aes_unit_encode(raw, processed, expanded_key);
        if(fwrite(processed,1,16,des) != 16) return ERROR;
        /*切换到一下块*/
        byte_len = byte_len_next;
        memcpy(raw,raw_next,16);
	}
	fclose(origin);
	fclose(des);
	return OK;
}

int aes_file_decode(const char* origin_path,const char* des_path, uint32_t* key)
{/*解密文件，使用密文挪用技术解决短块，请不要解密小于8字节的文件*/
    int i;
	int byte_len, byte_len_next;
    uint8_t raw[16], raw_next[16], processed[16], tmp[32];
    uint32_t expanded_key[44];
	FILE* origin;
	FILE* des;

    origin = fopen(origin_path, "rb");
    des = fopen(des_path, "wb");
	if(origin == NULL||des == NULL)
		return ERROR;

    aes_key_expand(key, expanded_key);
    byte_len = fread(raw, 1, 16, origin);
    if(byte_len < 8)
        return ERROR;
        
	for(;;)
	{
		byte_len_next = fread(raw_next, 1, 16, origin);
		if(byte_len_next < 16)
		{/*最后两块处理部分*/
            if(byte_len_next > 0)
            {
            	memcpy(tmp, raw, 16);
            	memcpy(tmp + 16, raw_next, byte_len_next); 
            	
                    #ifdef DEBUG
                    print_byte(tmp, 16 + byte_len_next, "last two block:");
                    #endif
            	
            	memcpy(raw_next, tmp + byte_len_next, 16);
                aes_unit_decode(raw_next, processed, expanded_key);
                memcpy(raw + byte_len_next, processed + byte_len_next, 16 - byte_len_next);
                memcpy(tmp, processed, byte_len_next);
                aes_unit_decode(raw, processed, expanded_key);
                if(fwrite(processed, 1, 16, des) != 16) return ERROR;
                if(fwrite(tmp, 1, byte_len_next, des) != byte_len_next) return ERROR;
            }
            else
            {
                aes_unit_decode(raw, processed, expanded_key);
                if(fwrite(processed, 1, 16, des) != 16) return ERROR; 
            }
            break; 
		}
		aes_unit_decode(raw, processed, expanded_key);
		if(fwrite(processed,1,16,des) != 16) return ERROR;
        
        byte_len = byte_len_next;
        memcpy(raw, raw_next, 16);
	}
	fclose(origin);
	fclose(des);
	return OK;
}

int main()
{
	/* 
	uint8_t k[16] = {0x00,0x01,0x20,0x01,0x71,0x01,0x98,0xae,0xda,0x79,0x17,0x14,0x60,0x15,0x35,0x94};
	uint32_t key[4];
	uint32_t expanded_key[44];
	uint8_t data[16] = {0x00,0x01,0x00,0x01,0x01,0xa1,0x98,0xaf,0xda,0x78,0x17,0x34,0x86,0x15,0x35,0x66};
    uint8_t cipher[16];
    uint8_t re[16];
	memcpy(key,k,16);
	//memset(data,0x1f,16);
	//aes_generate_key(key,4);
	aes_key_expand(key,4,expanded_key);
	
	#ifdef DEBUG
	print_byte((uint8_t*)expanded_key,176,"expanded_key:");
	#endif
    
	aes_unit_encode(data,cipher,expanded_key);
	print_byte((uint8_t*)key,16,"key:");
    print_byte(cipher,16,"cipher:");
    aes_unit_decode(cipher,re,expanded_key);
    print_byte(data,16,"origin_data");
	
	print_byte(re,16,"decrypted_data:");
	*/ 
	/*
	uint8_t a = 0x02;
	uint8_t b = 0x87;
	printf("%x\n",mul_gf2_8(b,a));
	*/
	
	
	uint8_t k[16] = {0x00,0x01,0x20,0x01,0x71,0x01,0x98,0xae,0xda,0x79,0x17,0x14,0x60,0x15,0x35,0x94};
	uint32_t key[4];
	clock_t start,finish;
	float time_used;
	
	start = clock(); 
    //aes_generate_keyfile("aes.key");
    aes_read_keyfile("aes.key",key);
	aes_file_encode("test.zip","test.aes",key);
	aes_file_decode("test.aes","aes_re.zip",key); 
	finish = clock();
	 
	time_used = (double)(finish - start) / 1000;
	printf("time:%fs\n",time_used);
    getchar();
    
	return 0;
 } 

