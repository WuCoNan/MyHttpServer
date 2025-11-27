#include "HttpSession.hpp"

HttpSession::HttpSession(const std::string& sessionId)
    :sessionId_(sessionId)
{
    refresh();
}

void HttpSession::refresh()
{
    lastActiveTime_=TimeStamp{TimeStamp::Now()};
}

bool HttpSession::isExpired() const
{
    return lastActiveTime_+KeepAliveTime<TimeStamp{TimeStamp::Now()};
}

const std::string& HttpSession::getSessionId() const
{
    return sessionId_;
}

void HttpSession::setData(const std::string& key,const std::string& value)
{
    data_[key]=value;
}
const std::string& HttpSession::getData(const std::string& key) const
{
    auto it=data_.find(key);
    if(it!=data_.end())
    {
        return it->second;
    }
    return "";
    
}