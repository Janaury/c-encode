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
*3. 以下划线分隔接所取区间的高位编号（最右边为最低位，最低位编号为1）
*4. 在以下划线分隔低位编号（包括起始和结束编号的bit）
*注意，若只取一个bit则没有第四步
*/
#define M8_8    0x80
#define M8_1    0x01
#define M8_6    0x20
#define M8_5_2  0x1e
#define M8_4_1  0x0f    //低4位
#define M8_8_5  0xf0    //高4位
#define M8_8_3  0xfc    //高6位
#define M8_6_1  0x3f    //低6位
#define M8_2_1  0X03
#define M8_8_7  0xc0

/*状态信息*/
#define ERROR     -1
#define OK        1

#define ENCRYPT  1
#define DECRYPT  0

/*数据类型定义*/
typedef unsigned long long uint64_t;
typedef unsigned long  uint32_t;
typedef unsigned char  uint8_t;
typedef struct subkey{
  uint8_t byte[6];
}Subkey;
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
*des_file_encode(const char* origin_path,const char* des_path,uint64_t key)
*加密一个文件
*参数为原文件路径，处理后文件的路径，密钥
*返回-1表示失败，返回1表示成功
*
*des_file_decode(const char* origin_path,const char* des_path,uint64_t key)
*解密一个文件，与加密类似
*/
int des_generate_keyfile(const char* keyfile_path);
int des_read_keyfile(const char* keyfile_path, uint8_t* key);
int des_file_encode(const char* origin_path,const char* des_path,uint8_t* key);
int des_file_decode(const char* origin_path,const char* des_path,uint8_t* key);

/*较内层的函数*/
void des_unit_process(uint8_t* raw,uint8_t* processed,Subkey* subkey,int mode);
void des_generate_subkey(uint8_t* main_key, Subkey* subkey_buff_address);
void des_generate_key(uint8_t* key_buff_address);
