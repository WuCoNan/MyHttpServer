#pragma once
#include <memory>
#include <mutex>
#include <random>
#include "HttpSession.hpp"
class HttpRequest;
class HttpResponse;
class HttpSessionManager
{
public:
    using HttpSessionPtr=std::shared_ptr<HttpSession>;
    using UniqueLock=std::unique_lock<std::mutex>;

    HttpSessionPtr getSession(const HttpRequest& request, HttpResponse* response);
    HttpSessionPtr createSession();
private:
    std::string generateSessionId();
    std::unordered_map<std::string,HttpSessionPtr> sessions_;
    std::mutex mutex_;
    std::mt19937 rng_{std::random_device{}()};
};