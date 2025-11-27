#include "Router.hpp"

bool Router::route(const HttpRequest& request, HttpResponse* response)
{
    RouteKey key{request.path(),request.method()};
    // First try to find exact match in handlers
    auto handlerIt=handlers_.find(key);
    if(handlerIt!=handlers_.end())
    {
        auto handler=handlerIt->second;
        if(handler)
        {
            handler->handle(request,response);
            return true;
        }
        return false;
    }
    // Then try to find exact match in callbacks
    auto callbackIt=callbacks_.find(key);
    if(callbackIt!=callbacks_.end())
    {
        auto callback=callbackIt->second;
        if(callback)
        {
            callback(request,response);
            return true;
        }
        return false;
    }
    // Next try regex handlers
    for(const auto&[method,pathRegex,handler]:regexHandlers_)
    {
        std::smatch matchResult;
        if(method==request.method()&&std::regex_match(request.path(),matchResult,pathRegex))
        {
            HttpRequest reqCopy=request;
            extractPathParams(matchResult,&reqCopy);
            if(handler)
            {
                handler->handle(reqCopy,response);
                return true;
            }
            return false;
        }
    }
    // Finally try regex callbacks
    for(const auto&[method,pathRegex,callback]:regexCallbacks_)
    {
        std::smatch matchResult;
        if(method==request.method()&&std::regex_match(request.path(),matchResult,pathRegex))
        {
            HttpRequest reqCopy=request;
            extractPathParams(matchResult,&reqCopy);
            if(callback)
            {
                callback(reqCopy,response);
                return true; 
            }
            return false;
        }
    }
    return false;

}