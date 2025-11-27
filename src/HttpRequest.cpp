#include "HttpRequest.hpp"
#include <algorithm>
bool HttpRequest::setMethod(const char *start, const char *end)
{
    std::string m(start, end);
    if (m == "GET")
        method_ = Method::GET;
    else if (m == "POST")
        method_ = Method::POST;
    else if (m == "HEAD")
        method_ = Method::HEAD;
    else if (m == "PUT")
        method_ = Method::PUT;
    else if (m == "DELETE")
        method_ = Method::DELETE;
    else
    {
        method_ = Method::INVALID;
        return false;
    }
    return true;
}

bool HttpRequest::setVersion(const char *start, const char *end)
{
    std::string ver(start, end);
    if (ver == "HTTP/1.0" || ver == "HTTP/1.1")
    {
        version_ = ver;
        return true;
    }
    else
    {
        return false;
    }
}

void HttpRequest::setQueryParams(const char *start, const char *end)
{
    const char* andPos;
    while((andPos=std::find(start,end,'&'))!=end)
    {
        const char* equalPos=std::find(start,andPos,'=');
        if(equalPos!=andPos)
        {
            std::string key(start,equalPos);
            std::string value(equalPos+1,andPos);
            queryParams_[key]=value;
        }
        start=andPos+1;
    }
    
    if(start!=end)
    {
        const char* equalPos=std::find(start,end,'=');
        if(equalPos!=end)
        {
            std::string key(start,equalPos);
            std::string value(equalPos+1,end);
            queryParams_[key]=value;
        }
    }
}

std::string HttpRequest::getSessionId() const
{
    auto it=headers_.find("Cookie");
    if(it!=headers_.end())
    {
        const std::string& cookie=it->second;
        size_t pos=cookie.find("SESSIONID=");
        if(pos!=std::string::npos)
        {
            size_t endPos=cookie.find(';',pos);
            if(endPos==std::string::npos)
            {
                endPos=cookie.length();
            }
            return cookie.substr(pos+10,endPos-(pos+10));
        }
    }
    return "";
}