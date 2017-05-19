#ifndef JSON_PARSER_H_
#define JSON_PARSER_H_

#include "json/json.h"


class JsonParser
{
public:
    JsonParser();
    
    ~JsonParser();
    
public:
    
    bool Parse(const char* file);
    
    int GetInteger(const std::string & key, int default_value = 0);
    
    bool GetBoolean(const std::string & key, bool default_value = false);
    
    double GetDouble(const std::string & key, double default_value = 0.0);
    
    std::string GetString(const std::string & key, const std::string & default_value = "");
    
    Json::Value GetJsonValue(const std::string & key);
    
private:
    Json::Value root_;
};

#endif