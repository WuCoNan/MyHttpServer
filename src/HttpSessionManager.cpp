#include "HttpSessionManager.hpp"

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sstream>
HttpSessionManager::HttpSessionPtr HttpSessionManager::getSession(const HttpRequest& request, HttpResponse* response)
{
    UniqueLock lock(mutex_);
    auto sessionId=request.getSessionId();
    if(sessionId.empty()||sessions_.find(sessionId)==sessions_.end()||sessions_[sessionId]->isExpired())
    {
        sessions_.erase(sessionId);

        auto newSession=createSession();
        response->addHeader("Set-Cookie","SESSIONID="+newSession->getSessionId()+"; Path=/; HttpOnly");
        return newSession;
    }
    sessions_[sessionId]->refresh();
    return sessions_[sessionId];
    
}

HttpSessionManager::HttpSessionPtr HttpSessionManager::createSession()
{
    auto sessionId=generateSessionId();
    auto session=std::make_shared<HttpSession>(sessionId);
    sessions_[sessionId]=session;
    return session;
}

std::string HttpSessionManager::generateSessionId()
{
    std::uniform_int_distribution<int> dist(0,15);
    while(true)
    {
        std::stringstream ss;
        for(int i=0;i<16;++i)
        {
            ss << std::hex << dist(rng_);
        }
        std::string sessionId=ss.str();
        if(sessions_.find(sessionId)==sessions_.end())
        {
            return sessionId;
        }
    }
    return "";
}