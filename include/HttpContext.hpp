#pragma once
#include "HttpRequest.hpp"

class Buffer;

class HttpContext
{
public:
    enum class ParseState
    {
        ExpectRequestLine,
        ExpectHeaders,
        ExpectBody,
        GotAll
    };
    HttpContext()
        : state_(ParseState::ExpectRequestLine)
    {
    }
    HttpRequest& request()
    {
        return request_;
    }
    const HttpRequest& request() const
    {
        return request_;
    }
    bool gotAll() const
    {
        return state_ == ParseState::GotAll;
    }
    void reset()
    {
        state_ = ParseState::ExpectRequestLine;
        HttpRequest new_request;
        std::swap(request_, new_request);
    }
    bool parseRequest(Buffer* buffer);
    bool parseRequestLine(const char* start,const char* end);
    bool parseHeader(const char* start,const char* end);
    bool parseBody(const char* start,const char* end);
private:
    HttpRequest request_;
    ParseState state_;
    inline static const char CRLF[]="\r\n";
    static constexpr size_t CRLF_LEN=2;
};