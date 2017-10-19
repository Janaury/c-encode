/*charset:utf8*/

/*base64编码
* 
*base64_encode(raw_data,raw_data_length,&result)
*参数为需要编码的数据，需要编码数据长度，存放结果的数组指针（需要加&变为二级指针，因为函数内部会分配内存） 
*返回编码后数据的长度 
* 
*base64_decode(enc_data,enc_data_length,&result)
*参数为需要解码的数据，需要解码数据长度，存放结果的数组指针 （需要加&变为二级指针，因为函数内部会分配内存）
*返回解码后数据的长度 
* 
*base64_file_encode(raw_file_path,encode_file_path)
*参数为需要编码的文件路径（文件必须存在），重新编码后生成新文件的路径（若文件存在会被覆盖） 
*无返回值 
* 
*base64_file_decode（encode_file_path，raw_file_path）
*参数为需要解码的文件路径（文件必须存在），解码后生成的文件路径 （若文件存在会被覆盖） 
*无返回值
* 
*/
/*外部功能函数*/
int base64_encode(const unsigned char* raw_data,int sum,unsigned char** result);
int base64_decode(const unsigned char* enc_data,int sum,unsigned char** result);
void base64_file_encode(const char* origin_filepath,const char* encode_filepath);
void base64_file_decode(const char* encode_filepath,const char* origin_filepath);

/*内部实现用函数*/
void base64_unit_encode(const unsigned char* raw_data,unsigned char* enc_data);
int base64_unit_decode(const unsigned char* enc_data,unsigned char* raw_data);
/*base64编码*/ 


/*置换密码编码
*permutation_encode(plain_text,len,key,key_len,&result)
*@para1:字符串明文
*@para2:该明文的长度
*@para3:以字符串数字表示的密钥，即读取列的顺序
*@para4:密钥的长度，也是置换中矩阵行的长度
*@para5:用来接收结果的二级指针（传入参数时使用&+一级指针）
*返回加密后字符串的长度
*
*permutation_decode(cipher_text,len,key,key_len,&result)
*参数与加密函数类似，区别是第一个为密文字符串
*返回解密后明文的长度
*/
/*外部功能函数*/
int permutation_encode(const char* plain_text,int len,const char* key,int key_len,char** result);
int permutation_decode(char* cipher_text,int len,char* key,int key_len,char** result);

/*置换密码编码*/


/*des加密算法

*/

/*宏定义，用于取某些位置的bit
*命名规则：
*1. 以M开头，表示mask
*2. M后接数据总长度
*3. 以下划线分隔接所取区间的结束编号（最低位编号为1）
*4. 在以下划线分隔起始编号（包括起始和结束编号的bit）
*注意，若只取一个bit则没有第四步
*/
#define M8_1      0x01                
#define M8_6      0x20                
#define M8_5_2    0x3c                
#define M32_32    0x80000000
#define M32_29    0x10000000
#define M32_30_29 0x30000000          
#define M64_64    0x8000000000000000  
#define M64_32    0x0000000080000000  
#define M64_6_1   0x000000000000003f  
#define M64_56    0x0080000000000000

#define ERROR     -1
#define OK        1
/*数据类型定义*/
typedef unsigned long long uint64_t;
typedef unsigned long  uint32_t;
typedef unsigned char  uint8_t;

/*函数定义*/

/*外部功能函数
*des_generate_keyfile(const char* keyfile_path)
*生成一个密钥文件
*参数为生成密码文件的路径，直接写文件名就在当前目录下生成
*返回-1表示失败，返回1表示成功
*
*des_read_keyfile(const char* keyfile_path, uint64_t* key)
*读取一个密钥文件
*参数为密码文件的路径，用于存放读取结果的指针
*返回-1表示失败，返回1表示成功
*
*des_file_code(const char* origin_path,const char* des_path,uint64_t key,int mode)
*加密一个文件
*参数为原文件路径，处理后文件的路径，密钥和运算模式（1表示加密，0表示解密）
*返回-1表示失败，返回1表示成功
*/
int des_generate_keyfile(const char* keyfile_path);
int des_read_keyfile(const char* keyfile_path, uint64_t* key);
int des_file_code(const char* origin_path,const char* des_path,uint64_t key,int mode);

/*较内层的函数
*des_get_key()
*生成一个密钥
*无需参数，返回一个uint64_t的密钥（64bit）
*
*des_add_check(uint8_t* key_byte)
*给密钥添加校验位
*参数为一个char，根据前7bit中1的数量设置最后一位
*
*des_get_sub_key(uint64_t key,uint64_t* sub_key)
*根据密钥计算出16个子密钥
*参数为64bit的密钥和用于存放生成的16个子密钥的数组
*
*des_unit_code(uint64_t raw,uint64_t* sub_key,uint64_t* processed,int mode)
*加密一个单元
*参数为原始数据，子密钥数组，用于接收处理后的数据的指针，运算模式（1表示加密，0表示解密）
*/
uint64_t des_get_key();
void des_add_check(uint8_t* key_byte);
void des_get_sub_key(uint64_t key,uint64_t* sub_key);
void des_unit_code(uint64_t raw,uint64_t* sub_key,uint64_t* processed,int mode);
