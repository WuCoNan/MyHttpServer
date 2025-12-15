#pragma once
#include <unordered_map>
#include <memory>
#include <functional>   
#include <string>
#include <regex>
#include "RouterHandler.hpp"
#include "HttpRequest.hpp"

class HttpResponse;

class Router
{
public:
    using RouterHandlerPtr=std::shared_ptr<RouterHandler>;
    using RouterHandlerCallback=std::function<void(const HttpRequest&,HttpResponse*)>;

    struct RouteKey
    {
        std::string path;
        HttpRequest::Method method;
        bool operator==(const RouteKey& other) const
        {
            return path==other.path&&method==other.method;
        }
    };

    struct RouteKeyHash
    {
        std::size_t operator()(const RouteKey& key) const
        {
            auto pathHash=std::hash<std::string>{}(key.path);
            auto methodHash=std::hash<int>{}(static_cast<int>(key.method));
            return methodHash*31+pathHash;
        }
    };
    struct RouteHandlerObj
    {
        HttpRequest::Method method;
        std::regex pathRegex;
        RouterHandlerPtr handler;
    };
    struct RouteCallbackObj
    {
        HttpRequest::Method method;
        std::regex pathRegex;
        RouterHandlerCallback callback;
    };
    void registerHandler(HttpRequest::Method method, const std::string& path, RouterHandlerPtr handler)
    {
        RouteKey key{path,method};
        handlers_[key]=handler;
    }
    void registerCallback(HttpRequest::Method method, const std::string& path, RouterHandlerCallback cb)
    {
        RouteKey key{path,method};
        callbacks_[key]=cb;
    }
    void addRegexHandler(HttpRequest::Method method, const std::string& pathRegex, RouterHandlerPtr handler)
    {
        regexHandlers_.push_back(RouteHandlerObj{method,std::regex(pathRegex),handler});
    }
    void addRegexCallback(HttpRequest::Method method, const std::string& pathRegex, RouterHandlerCallback cb)
    {
        regexCallbacks_.push_back(RouteCallbackObj{method,std::regex(pathRegex),cb});
    }
    bool route(const HttpRequest& request, HttpResponse* response);
private:
    std::unordered_map<RouteKey,RouterHandlerCallback,RouteKeyHash> callbacks_;
    std::unordered_map<RouteKey,RouterHandlerPtr,RouteKeyHash> handlers_;
    std::vector<RouteHandlerObj> regexHandlers_;
    std::vector<RouteCallbackObj> regexCallbacks_;

    void extractPathParams(const std::smatch& matchResult, HttpRequest* request)
    {
        for(size_t i=1;i<matchResult.size();++i)
        {
            request->setPathParams("param"+std::to_string(i),matchResult[i]);
        }
    }
};