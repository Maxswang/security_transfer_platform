#ifndef __BASE64_H__
#define __BASE64_H__
#include <string>

std::string base64_decode(const std::string &ascdata);
std::string base64_encode(const std::string &bindata);

#endif

