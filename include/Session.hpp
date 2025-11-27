#pragma once
#include <memory>
#include <string>
#include "TcpConnection.hpp"
#include "Buffer.hpp"
#include "TcpClient.hpp"
class Client;
class EventLoop;
class TcpClient;

class Session:public Noncopyable
{
public:
    Session(Client* owner,EventLoop* loop);
    void Start();
    void Stop();
private:
    Client* owner_;
    EventLoop* loop_;
    std::unique_ptr<TcpClient> tcpClient_;
    
    void OnMessage(const std::shared_ptr<TcpConnection>&, Buffer *);
    void OnConnection(const std::shared_ptr<TcpConnection>&);
};