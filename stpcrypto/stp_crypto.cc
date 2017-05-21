#include "stp_crypto.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "stputil/security/aes.h"
#include "stputil/security/aes_locl.h"
#include "stputil/security/base64.h"
#include "stputil/security/md5.h"

using std::string;

static bool GetStpCryptKey(std::string& key)
{
    bool res = false;
    key = "12345678901234567890123456789012";
    return res;
}

uint32_t ReadUint32(uchar_t *buf)
{
    uint32_t data = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    return data;
}

void WriteUint32(uchar_t *buf, uint32_t data)
{
    buf[0] = static_cast<uchar_t>(data >> 24);
    buf[1] = static_cast<uchar_t>((data >> 16) & 0xFF);
    buf[2] = static_cast<uchar_t>((data >> 8) & 0xFF);
    buf[3] = static_cast<uchar_t>(data & 0xFF);
}


#ifdef __cplusplus
extern "C" {
#endif

// 根据标记获取token和密钥，打包数据，加密

int EncryptPlain(const char* plain, uint32_t plain_len, char** cipher, uint32_t& cipher_len)
{
    if(plain == NULL|| plain_len <=0 )
    {
        return -1;
    }
    
    uint32_t remain = plain_len % 16;
    uint32_t blocks = (plain_len + 15) / 16;
    
    if (remain > 12 || remain == 0) {
        blocks += 1;
    }
    uint32_t encrypt_len = blocks * 16;
    
    unsigned char* pData = (unsigned char*) calloc(encrypt_len, 1);
    memcpy(pData, plain, plain_len);
    unsigned char* pEncData = (unsigned char*) malloc(encrypt_len);
    
    WriteUint32((pData + encrypt_len - 4), plain_len);
    AES_KEY aesKey;
    
    std::string key;
    GetStpCryptKey(key);
    AES_set_encrypt_key((const unsigned char*)key.c_str(), 256, &aesKey);
    for (uint32_t i = 0; i < blocks; i++) {
        AES_encrypt(pData + i * 16, pEncData + i * 16, &aesKey);
    }
    
    free(pData);
    string strEnc((char*)pEncData, encrypt_len);
    free(pEncData);
    string strDec = base64_encode(strEnc);
    cipher_len = (uint32_t)strDec.length();
    
    char* pTmp = (char*) malloc(cipher_len + 1);
    memcpy(pTmp, strDec.c_str(), cipher_len);
    pTmp[cipher_len] = 0;
    *cipher = pTmp;
    return 0;
}


int DecryptCipher(const char* cipher, uint32_t cipher_len, char** plain, uint32_t& plain_len)
{
    if(cipher == NULL|| cipher_len <=0 )
    {
        return -1;
    }
    
    std::string strResult = base64_decode(std::string(cipher, cipher_len));
    uint32_t nLen = (uint32_t)strResult.length();
    if(nLen == 0)
    {
        return -2;
    }
    
    const unsigned char* pData = (const unsigned char*) strResult.c_str();
    
    if (nLen % 16 != 0) {
        return -3;
    }
    // 先申请nLen 个长度，解密完成后的长度应该小于该长度
    char* pTmp = (char*)malloc(nLen + 1);
    
    uint32_t blocks = nLen / 16;
    AES_KEY aesKey;
    
    std::string key;
    GetStpCryptKey(key);
    AES_set_decrypt_key((const unsigned char*) key.c_str(), 256, &aesKey);           //设置AES解密密钥
    for (uint32_t i = 0; i < blocks; i++) {
        AES_decrypt(pData + i * 16, (unsigned char*)pTmp + i * 16, &aesKey);
    }
    
    uchar_t* pStart = (uchar_t*)pTmp+nLen-4;
    plain_len = ReadUint32(pStart);
    //        printf("%u\n", nOutLen);
    if(plain_len > nLen)
    {
        free(pTmp);
        return -4;
    }
    pTmp[plain_len] = 0;
    *plain = pTmp;
    return 0;
}

void Free(char* ptr)
{
    if(ptr)
    {
        free(ptr);
        ptr = NULL;
    }
}

    
#ifdef __cplusplus
}
#endif