#include "TcpClient.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"
#include "Logger.hpp"
#include <arpa/inet.h>
TcpClient::TcpClient(EventLoop* loop)
                    :loop_(loop)
                    ,clientFd_(CreateClientFd())
{

}

void TcpClient::Connect()
{
    sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8080);
    inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr);

    if(::connect(clientFd_,reinterpret_cast<sockaddr*>(&server_addr),sizeof(server_addr))<0)
        LOG_ERROR("TcpClient   connect   error\n");
    
    conn_=std::make_shared<TcpConnection>(loop_,clientFd_);
    conn_->SetMessageCallback(messageCallback_);
    conn_->SetOnConnectionCallback(onConnectionCallback_);
    conn_->SetCloseCallback(std::bind(&TcpClient::RemoveConnection,this,std::placeholders::_1));

    loop_->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished,conn_));
}

void TcpClient::DisConnect()
{
    conn_->ShutDown();
}

void TcpClient::RemoveConnection(const std::shared_ptr<TcpConnection> &conn)
{
    loop_->RunInLoop(std::bind(&TcpClient::RemoveConnectionInLoop,this,conn));
}

void TcpClient::RemoveConnectionInLoop(const std::shared_ptr<TcpConnection> &conn)
{
    if(closeCallback_)
        closeCallback_(conn);
}