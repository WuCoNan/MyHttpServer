#include "Session.hpp"
#include "EventLoop.hpp"

#include "Client.hpp"
Session::Session(Client* owner,EventLoop* loop)
                :owner_(owner)
                ,loop_(loop)
                ,tcpClient_(new TcpClient(loop))
{

}

void Session::Start()
{
    tcpClient_->Connect();
}

void Session::Stop()
{
    tcpClient_->DisConnect();
}

void Session::OnMessage(const std::shared_ptr<TcpConnection>& conn, Buffer *buffer)
{
    auto buf=buffer->RetriveAllAsString();
    conn->Send(buf.data(),buf.size());
}

void Session::OnConnection(const std::shared_ptr<TcpConnection>& conn)
{
    auto message=owner_->Message();
    conn->Send(message.data(),message.size());
}
