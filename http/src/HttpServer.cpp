#include "HttpServer.hpp"
#include "TcpConnection.hpp"
#include "Logger.hpp"
#include "HttpResponse.hpp"
#include <openssl/err.h>
HttpServer::HttpServer(bool useSSL)
    : server_(&mainLoop_), useSSL_(useSSL)
{
    server_.SetMessageCallback(std::bind(&HttpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2));
    server_.SetConnectionCallback(std::bind(&HttpServer::onConnection,this,std::placeholders::_1));
    if(useSSL_)
    {
        initSSL();
    }
}

void HttpServer::initSSL()
{
    sslCtx_=std::unique_ptr<SSL_CTX, SslCtxDeleter>(SSL_CTX_new(TLS_server_method()));

    if (!sslCtx_) 
    {
        LOG_ERROR("Failed to create SSL context\n");
        return;
    }

    SSL_CTX_set_min_proto_version(sslCtx_.get(),TLS1_2_VERSION);
    //SSL_CTX_set_max_proto_version(sslCtx_.get(),TLS1_2_VERSION);

    SSL_CTX_set_cipher_list(sslCtx_.get(),
    "ECDHE-ECDSA-AES256-GCM-SHA384:"
    "ECDHE-RSA-AES256-GCM-SHA384:"
    "DHE-RSA-AES256-GCM-SHA384");

    SSL_CTX_set1_curves_list(sslCtx_.get(),"X25519:prime256v1:secp384r1");

    SSL_CTX_set_security_level(sslCtx_.get(),2);

    if(SSL_CTX_use_certificate_file(sslCtx_.get(), "../ssl/localhost.crt", SSL_FILETYPE_PEM)<=0)
    {
        LOG_ERROR("HttpServer:   load   certificate   error\n");
        return;
    }

    if(SSL_CTX_use_PrivateKey_file(sslCtx_.get(), "../ssl/localhost.key", SSL_FILETYPE_PEM)<=0)
    {
        LOG_ERROR("HttpServer:   load   private   key   error\n");

        return;
    }

    const SSL_CTX* ctx=sslCtx_.get();
    
    if(!SSL_CTX_check_private_key(ctx))
    {
        LOG_ERROR("HttpServer:   Private   key   does   not   match   the   public   certificate\n");
    }
    LOG_INFO("HttpServer:   SSL   context   initialized   successfully\n");
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
        if(useSSL_)
        {
            std::unique_lock<std::mutex> lock(sslConnMutex_);
            sslConnections_[conn]=std::make_unique<SslConnection>(sslCtx_.get(),conn);
            sslConnections_[conn]->setOriginalMessageCallback(std::bind(&HttpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2));
        }
        LOG_INFO("HttpServer:   new   connection  %d  established\n",conn->GetFd());

    }
        
    else
    {
        if(useSSL_)
        {
            std::unique_lock<std::mutex> lock(sslConnMutex_);
            sslConnections_.erase(conn);
        }
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

    sendResponse(conn,response);

    if(response.isCloseConnection())
    {
        conn->ShutDown();
    }
}


void HttpServer::sendResponse(const std::shared_ptr<TcpConnection>& conn, const HttpResponse& response)
{
    Buffer buffer;
    response.appendToBuffer(&buffer);
    if(useSSL_)
    {
        SslConnection* sslConn=nullptr;
        {
            std::unique_lock<std::mutex> lock(sslConnMutex_);
            auto it=sslConnections_.find(conn);
            if(it!=sslConnections_.end())
            {
                sslConn=it->second.get();
            }
        }
        if(sslConn)
        {
            sslConn->onEcryption(&buffer);
        }
    }
    
    conn->Send(buffer.RetriveAllAsString().data(),buffer.ReadableBytes());
}