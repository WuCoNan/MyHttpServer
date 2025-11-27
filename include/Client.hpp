#pragma once
#include <string>
#include "EventLoopThreadPool.hpp"
#include "Session.hpp"
class Client
{
public:
    Client(EventLoop* loop,int threadCount, int sessionCount);
    std::string Message() const { return message_; }

private:
    using SessionList = std::vector<std::unique_ptr<Session>>;
    EventLoop* loop_;
    int threadCount_;
    int sessionCount_;
    std::string message_;
    std::unique_ptr<EventLoopThreadPool> pool_;
    SessionList sessions_;
    static constexpr int messageSize = 4096;
};