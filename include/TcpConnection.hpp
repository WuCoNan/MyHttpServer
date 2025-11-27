#pragma once
#include "Noncopyable.hpp"
#include "Callbacks.hpp"
#include "Buffer.hpp"
#include "HttpContext.hpp"
#include <memory>
#include <atomic>
#include <string>
class EventLoop;
class Channel;

class TcpConnection :public Noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, int fd);
    ~TcpConnection();
    void ConnectionEstablished();
    void ConnectionDestroyed();
    void SetMessageCallback(MessageCallback);
    void SetCloseCallback(CloseCallback);
    void SetOnConnectionCallback(OnConnectionCallback);
    void Send(const char* data,size_t len);
    void SendInLoop(const char* data,size_t len);
    void SendInLoop(const std::string& data)
    {
        SendInLoop(data.data(),data.size());
    }
    void ShutDown();
    void ShutDownInLoop();
    EventLoop* Loop() const {return loop_;};
    HttpContext& GetHttpContext() { return context_; }
    bool IsConnected() const { return state_ == CONNECTED; }
    int GetFd() const;
private:
    enum STATE
    {
        CONNECTED=0,
        DISCONNECTED,
        DISCONNECTING,
        CONNECTING
    };
    std::unique_ptr<Channel> channel_;
    EventLoop *loop_;
    MessageCallback message_cb_;
    CloseCallback close_cb_;
    OnConnectionCallback on_conn_cb_;
    Buffer read_buffer_;
    Buffer write_buffer_;
    std::atomic<int> state_;
    HttpContext context_;

    void HandleRead();
    void HandleWrite();
    void HandleClose();
    void HandleError();
};