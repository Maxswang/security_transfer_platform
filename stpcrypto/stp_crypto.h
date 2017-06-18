#ifndef STP_INTERFACE_H_
#define STP_INTERFACE_H_


#ifdef WIN32
typedef char			int8_t;
typedef short			int16_t;
typedef int				int32_t;
typedef	long long		int64_t;
typedef unsigned char	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;
typedef	unsigned long long	uint64_t;
typedef int				socklen_t;
#else
#include <stdint.h>
#endif
typedef unsigned char	uchar_t;

#ifdef WIN32
#define DLL_MODIFIER __declspec(dllexport)
#else
#define DLL_MODIFIER
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
*  
*
*  @param clnt_or_svr  0:client, 1 svr
*  @param plain_len   待加密消息内容长度
*  @param cipher 加密后的文本
*  @param cipher_len  加密后的文本长度
*
*  @return 返回 0-成功; 其他-失败
*/
DLL_MODIFIER int InitStpCrypto(int clnt_or_svr);

/**
*  对消息加密
*
*  @param plain  待加密的消息内容指针
*  @param plain_len   待加密消息内容长度
*  @param cipher 加密后的文本
*  @param cipher_len  加密后的文本长度
*
*  @return 返回 0-成功; 其他-失败
*/
DLL_MODIFIER int EncryptPlain(const char* plain, 
                              uint32_t plain_len, 
                              char** cipher, 
                              uint32_t& cipher_len,
                              const char* token);
/**
*  对消息解密
*
*  @param cipher  待解密的消息内容指针
*  @param cipher_len   待解密消息内容长度
*  @param plain 解密后的文本
*  @param plain_len  解密后的文本长度
*  @param token  
*
*  @return 返回 0-成功; 其他-失败
*/
DLL_MODIFIER int DecryptCipher(const char* cipher, 
                               uint32_t cipher_len, 
                               char** plain, 
                               uint32_t& plain_len,
                               char** token);

/**
*  释放资源
*
*  @param ptr 需要释放的资源
*/
DLL_MODIFIER void Free(char* ptr);
    
    
#ifdef __cplusplus
}
#endif


#endif