#include "Client.hpp"

Client::Client(EventLoop* loop,int threadCount,int sessionCount)
              :loop_(loop)
              ,threadCount_(threadCount)
              ,sessionCount_(sessionCount)
              ,message_(messageSize,'a')
              ,pool_(new EventLoopThreadPool())
{
    pool_->SetNum(threadCount);
    pool_->Start();

    for(int i=0;i<sessionCount;i++)
    {
        auto loop=pool_->GetNextLoop();
        sessions_.emplace_back(std::make_unique<Session>(this,loop));
        sessions_.back()->Start();
    }
}