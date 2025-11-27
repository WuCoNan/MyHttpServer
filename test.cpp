#include "TcpServer.hpp"
#include "Buffer.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"
#include "Logger.hpp"
#include "HttpContext.hpp"
#include "HttpServer.hpp"
#include "GomokuServer.hpp"
#include <iostream>
#include <cstring>
void MessageFunc(const std::shared_ptr<TcpConnection>& conn, Buffer *buffer)
{
    auto str = buffer->RetriveAllAsString();
    //std::cout << "server    has    received : " << str << std::endl;
    LOG_INFO("server    has    received   %d   bytes   data\n",str.size());
    conn->Send(str.data(),str.size());
    
    //conn->ShutDown();
}

void TestOn()
{
    GomokuServer server;
    server.setThreadNum(3);
    server.start();
}

int main()
{
    auto logger=Logger::GetInstance();
    logger->SetType(INFO);
    
    TestOn();

    return 0;
}