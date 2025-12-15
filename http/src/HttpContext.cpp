#include "HttpContext.hpp"
#include "Buffer.hpp"
#include <algorithm>
bool HttpContext::parseRequest(Buffer* buffer)
{
    bool ok =true;
    bool hasMore =true;
    while(hasMore)
    {
        if(state_==ParseState::ExpectRequestLine)
        {
            const char* crlf=buffer->Find(CRLF,CRLF_LEN);
            if(crlf)
            {
                ok=parseRequestLine(buffer->BeginRead(),crlf);
                if(ok)
                {
                    buffer->RetriveUntil(crlf+CRLF_LEN);
                    state_=ParseState::ExpectHeaders;
                }
                else
                {
                    hasMore=false;
                }
                
            }
            else
            {
                hasMore=false;
            }
        }
        // Further parsing for headers and body would go here
        else if(state_==ParseState::ExpectHeaders)
        {
            while(true)
            {
                const char* crlf=buffer->Find(CRLF,CRLF_LEN);
                if(crlf)
                {
                    if(crlf==buffer->BeginRead())
                    {
                        buffer->RetriveUntil(crlf+CRLF_LEN);
                        if(request_.contentLength()>0)
                        {
                            state_=ParseState::ExpectBody;
                        }
                            
                        else
                        {
                            state_=ParseState::GotAll;
                            hasMore=false;
                        }
                            
                        break;
                    }
                    else
                    {
                        ok=parseHeader(buffer->BeginRead(),crlf);
                        if(ok)
                        {
                            buffer->RetriveUntil(crlf+CRLF_LEN);
                        }
                        else
                        {
                            hasMore=false;
                            break;
                        }
                    }
                 
                }
                else
                {
                    hasMore=false;
                    break;
                }
            }
        }
        else if(state_==ParseState::ExpectBody)
        {
            if(buffer->ReadableBytes()>=request_.contentLength())
            {
                ok=parseBody(buffer->BeginRead(),buffer->BeginRead()+request_.contentLength());
                if(ok)
                {
                    buffer->Retrieve(request_.contentLength());
                    state_=ParseState::GotAll;
                    hasMore=false;
                }
                else
                {
                    hasMore=false;
                }
            }
            else
            {
                hasMore=false;
            }
        }

        
    }
    return ok;
}

bool HttpContext::parseRequestLine(const char* start,const char* end)
{
    bool ok=false;
    const char* space=std::find(start,end,' ');
    if(space!=end&&request_.setMethod(start,space))
    {
        start=space+1;
        space=std::find(start,end,' ');
        if(space!=end)
        {
            const char* argumentPath=std::find(start,space,'?');
            if(argumentPath!=space)
            {
                request_.setPath(start,argumentPath);
                request_.setQueryParams(argumentPath+1,space);
            }
            else
            {
                request_.setPath(start,space);
            }
        }

        start=space+1;
        if(start!=end&&request_.setVersion(start,end))
        {
            ok=true;
        }
    }

    return ok;
}

bool HttpContext::parseHeader(const char* start,const char* end)
{
    bool ok=false;
    const char* colon=std::find(start,end,':');
    if(colon!=end)
    {
        std::string headerField(start,colon);
        ++colon;
        while(colon<end&&isspace(*colon))
        {
            ++colon;
        }
        std::string headerValue(colon,end);
        if(headerField=="Content-Length")
        {
            request_.setContentLength(std::stoul(headerValue));
        }
        else
        {
            request_.addHeader(headerField,headerValue);
        }
        ok=true;
    }
    return ok;
}

bool HttpContext::parseBody(const char* start,const char* end)
{
    bool ok=true;
    request_.setBody(start,end);
    return ok;
}