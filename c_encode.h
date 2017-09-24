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

int base64_encode(const unsigned char* raw_data,int sum,unsigned char** result);
int base64_decode(const unsigned char* enc_data,int sum,unsigned char** result);
void base64_file_encode(const char* origin_filepath,const char* encode_filepath);
void base64_file_decode(const char* encode_filepath,const char* origin_filepath);

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

int permutation_encode(const char* plain_text,int len,const char* key,int key_len,char** result);
int permutation_decode(char* cipher_text,int len,char* key,int key_len,char** result);

/*置换密码编码*/