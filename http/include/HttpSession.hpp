#pragma once
#include "TimeStamp.hpp"
#include <unordered_map>
#include <string>
class HttpSession
{
public:
    explicit HttpSession(const std::string& sessionId);
    bool isExpired() const;
    const std::string& getSessionId() const;
    void refresh();
    void setData(const std::string& key,const std::string& value);
    const std::string& getData(const std::string& key) const;
private:
    static const int KeepAliveTime=60*1000*1000; //us
    TimeStamp lastActiveTime_;
    std::unordered_map<std::string,std::string> data_;
    std::string sessionId_;
};