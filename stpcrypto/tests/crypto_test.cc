#include "stpcrypto/stp_crypto.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using std::string;
using namespace std;


int main()
{
    const string msg = "1234qwerty4321";
    char* out_buf;
    uint32_t out_len;
    if (EncryptPlain(msg.c_str(), msg.length(), &out_buf, out_len) != 0)
    {
        cout << "encrypt error!" << endl;
        return -1;
    }
    
    cout << "encrypt data :" << out_buf << endl;
    char* in_buf;
    uint32_t in_len;
    if (DecryptCipher(out_buf, out_len, &in_buf, in_len) != 0)
    {
        cout << "decrypt error!" << endl;
        Free(out_buf);
        return -1;
    }
    cout << "origin data :" << in_buf << endl;
    Free(out_buf);
    Free(in_buf);
    
    
    
    return 0;
}
