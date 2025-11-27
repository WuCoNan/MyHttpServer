#include "HttpServer.hpp"
#include "TcpConnection.hpp"
#include "Logger.hpp"
#include "HttpResponse.hpp"
HttpServer::HttpServer()
    : server_(&mainLoop_)
{
    server_.SetMessageCallback(std::bind(&HttpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2));
    server_.SetConnectionCallback(std::bind(&HttpServer::onConnection,this,std::placeholders::_1));
}

void HttpServer::onMessage(const std::shared_ptr<TcpConnection>& conn,Buffer* buffer)
{
    auto& context=conn->GetHttpContext();
    if(!context.parseRequest(buffer))
    {
        LOG_ERROR("HttpServer:   parseRequest   error\n");
        conn->ShutDown();
    }
    if(context.gotAll())
    {
        handleRequest(conn);
        context.reset();
    }
}

void HttpServer::onConnection(const std::shared_ptr<TcpConnection>& conn)
{
    if(conn->IsConnected())
    {
        LOG_INFO("HttpServer:   new   connection  %d  established\n",conn->GetFd());
    }
        
    else
    {
        LOG_INFO("HttpServer:   connection  %d  destroyed\n",conn->GetFd());

    }   

}
void HttpServer::handleRequest(const std::shared_ptr<TcpConnection>& conn)
{
    auto& context=conn->GetHttpContext();
    const auto& request=context.request();
    HttpResponse response;

    if(request.getHeader("Connection")=="close"||
       (request.version()=="HTTP/1.0"&&
        request.getHeader("Connection")!="keep-alive"))
    {
        response.setCloseConnection(true);
    }
    else
    {
        response.setCloseConnection(false);
    }

    response.setVersion(request.version());
    // Here we should route the request to get the response
    // For simplicity, we just return a 200 OK response with a simple body
    if(!router_.route(request,&response))
    {
        LOG_ERROR("HttpServer:   route   error\n");
        conn->ShutDown();
        return;
    }

    Buffer buffer;
    response.appendToBuffer(&buffer);
    conn->Send(buffer.RetriveAllAsString().data(),buffer.ReadableBytes());
    
    if(response.isCloseConnection())
    {
        conn->ShutDown();
    }
}