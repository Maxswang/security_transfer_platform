#include "json_parser.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>

JsonParser::JsonParser()
{
    
}

JsonParser::~JsonParser()
{
    
}

bool JsonParser::Parse(const char *file)
{
    std::ifstream ifs;
    ifs.open(file, std::ios::binary);
    
    if (!ifs.is_open())
    {
        fprintf(stderr, "open %s error in file %s at line %d !\n", file, __FILE__, __LINE__);
        return false;
    }
    
    Json::Reader reader;
    bool ret = reader.parse(ifs, root_);
    ifs.close();
    if (!ret)
    {
        fprintf(stderr, "parse %s error in file %s at line %d !\n", file, __FILE__, __LINE__);
        return false;
    }
    
    return true;
}

int JsonParser::GetInteger(const std::string &key, int default_value)
{
    return root_.get(key, default_value).asInt();
}

bool JsonParser::GetBoolean(const std::string &key, bool default_value)
{
    return root_.get(key, default_value).asBool();
}

double JsonParser::GetDouble(const std::string &key, double default_value)
{
    return root_.get(key.c_str(), default_value).asDouble();
}

std::string JsonParser::GetString(const std::string &key, const std::string &default_value)
{
    return root_.get(key.c_str(), default_value.c_str()).asCString();
}

Json::Value JsonParser::GetJsonValue(const std::string &key)
{
    return root_[key];
}
