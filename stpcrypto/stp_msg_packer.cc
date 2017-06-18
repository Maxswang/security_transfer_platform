#include "stp_msg_packer.h"
#include <assert.h>

bool StpMsgPacker::PackPlain(const char *plain, 
                             uint32_t plain_len, 
                             char **pack_buf, 
                             uint32_t &pack_len, 
                             const char *token, 
                             int clnt_or_svr)
{
    bool res = false;
    rpc::StpCryptoMsg msg;
    assert(plain != NULL);
    
    switch(clnt_or_svr)
    {
    case 0: // client
        break;
    case 1: // server
        break;
    default:
        break;
    }
    
    
    return res;
}
