#ifndef STP_MSG_PACKER_H_
#define STP_MSG_PACKER_H_

#include "codec/rpc.pb.h"
#include "stp_crypto_access.h"

class StpMsgPacker 
{
public:
    bool PackPlain(const char* plain, 
                          uint32_t plain_len, 
                          char** pack_buf, 
                          uint32_t& pack_len,
                          const char* token,
                          int clnt_or_svr);
    bool UnpackCipher(const char* cipher, 
                             uint32_t cipher_len, 
                             char** plain, 
                             uint32_t& plain_len);
    
private:
    StpCryptoAccess sca_;
};

#endif