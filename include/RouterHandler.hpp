#pragma once
class HttpRequest;
class HttpResponse;
class RouterHandler
{
public:
    virtual ~RouterHandler()=default;
    virtual void handle(const HttpRequest&,HttpResponse*)=0;
};