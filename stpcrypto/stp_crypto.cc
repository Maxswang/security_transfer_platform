#include "stp_crypto.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "stpcomm/security/aes.h"
#include "stpcomm/security/aes_locl.h"
#include "stpcomm/security/base64.h"
#include "stpcomm/security/md5.h"

using std::string;

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

int EncryptMsg(const char* pInData, uint32_t nInLen, char** ppOutData, uint32_t& nOutLen)
{
    if(pInData == NULL|| nInLen <=0 )
    {
        return -1;
    }
    uint32_t nRemain = nInLen % 16;
    uint32_t nBlocks = (nInLen + 15) / 16;
    
    if (nRemain > 12 || nRemain == 0) {
        nBlocks += 1;
    }
    uint32_t nEncryptLen = nBlocks * 16;
    
    unsigned char* pData = (unsigned char*) calloc(nEncryptLen, 1);
    memcpy(pData, pInData, nInLen);
    unsigned char* pEncData = (unsigned char*) malloc(nEncryptLen);
    
    WriteUint32((pData + nEncryptLen - 4), nInLen);
    AES_KEY aesKey;
    
    const char *key = "12345678901234567890123456789012";
    AES_set_encrypt_key((const unsigned char*)key, 256, &aesKey);
    for (uint32_t i = 0; i < nBlocks; i++) {
        AES_encrypt(pData + i * 16, pEncData + i * 16, &aesKey);
    }
    
    free(pData);
    string strEnc((char*)pEncData, nEncryptLen);
    free(pEncData);
    string strDec = base64_encode(strEnc);
    nOutLen = (uint32_t)strDec.length();
    
    char* pTmp = (char*) malloc(nOutLen + 1);
    memcpy(pTmp, strDec.c_str(), nOutLen);
    pTmp[nOutLen] = 0;
    *ppOutData = pTmp;
    return 0;
}

int DecryptMsg(const char* pInData, uint32_t nInLen, char** ppOutData, uint32_t& nOutLen)
{
    if(pInData == NULL|| nInLen <=0 )
    {
        return -1;
    }
    string strInData(pInData, nInLen);
    std::string strResult = base64_decode(strInData);
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
    
    uint32_t nBlocks = nLen / 16;
    AES_KEY aesKey;
    
    const char *key = "12345678901234567890123456789012";
    AES_set_decrypt_key((const unsigned char*) key, 256, &aesKey);           //设置AES解密密钥
    for (uint32_t i = 0; i < nBlocks; i++) {
        AES_decrypt(pData + i * 16, (unsigned char*)pTmp + i * 16, &aesKey);
    }
    
    uchar_t* pStart = (uchar_t*)pTmp+nLen-4;
    nOutLen = ReadUint32(pStart);
    //        printf("%u\n", nOutLen);
    if(nOutLen > nLen)
    {
        free(pTmp);
        return -4;
    }
    pTmp[nOutLen] = 0;
    *ppOutData = pTmp;
    return 0;
}

int EncryptPass(const char* pInData, uint32_t nInLen, char** ppOutData, uint32_t& nOutLen)
{
    if(pInData == NULL|| nInLen <=0 )
    {
        return -1;
    }
    char *pTmp = (char*)malloc(33);
    MD5_Calculate(pInData, nInLen, pTmp);
    pTmp[32] = 0;
    *ppOutData = pTmp;
    nOutLen = 32;
    return 0;
}

void Free(char* pOutData)
{
    if(pOutData)
    {
        free(pOutData);
        pOutData = NULL;
    }
}

    
#ifdef __cplusplus
}
#endif