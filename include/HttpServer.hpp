#pragma once
#include "TcpServer.hpp"
#include "EventLoop.hpp"
#include "Router.hpp"
#include "HttpRequest.hpp"
#include "HttpSessionManager.hpp"
class HttpServer
{
public:
    HttpServer();
    void setThreadNum(int num){server_.SetSubLoopNum(num);}
    void start(){server_.Start();mainLoop_.Loop();}
    void registerGet(const std::string& path,Router::RouterHandlerCallback cb)
    {
        router_.registerCallback(HttpRequest::Method::GET,path,cb);
    }
    void registerGet(const std::string& path,Router::RouterHandlerPtr handler)
    {
        router_.registerHandler(HttpRequest::Method::GET,path,handler);
    }
    void addRegexGet(const std::string& pathRegex,Router::RouterHandlerPtr handler)
    {
        router_.addRegexHandler(HttpRequest::Method::GET,pathRegex,handler);
    }
    void addRegexGet(const std::string& pathRegex,Router::RouterHandlerCallback cb)
    {
        router_.addRegexCallback(HttpRequest::Method::GET,pathRegex,cb);
    }
    void registerPost(const std::string& path,Router::RouterHandlerCallback cb)
    {
        router_.registerCallback(HttpRequest::Method::POST,path,cb);
    }
    void registerPost(const std::string& path,Router::RouterHandlerPtr handler)
    {
        router_.registerHandler(HttpRequest::Method::POST,path,handler);
    }
    void addRegexPost(const std::string& pathRegex,Router::RouterHandlerPtr handler)
    {
        router_.addRegexHandler(HttpRequest::Method::POST,pathRegex,handler);
    }
    void addRegexPost(const std::string& pathRegex,Router::RouterHandlerCallback cb)
    {
        router_.addRegexCallback(HttpRequest::Method::POST,pathRegex,cb);
    }

    void setSessionManager(std::unique_ptr<HttpSessionManager> sessionManager)
    {
        sessionManager_=std::move(sessionManager);
    }
    HttpSessionManager* getSessionManager() const
    {
        return sessionManager_.get();
    }
private:
    TcpServer server_;
    EventLoop mainLoop_;
    Router router_;
    std::unique_ptr<HttpSessionManager> sessionManager_;

    void onMessage(const std::shared_ptr<TcpConnection>&,Buffer*);
    void onConnection(const std::shared_ptr<TcpConnection>&);
    void handleRequest(const std::shared_ptr<TcpConnection>&);
    
};