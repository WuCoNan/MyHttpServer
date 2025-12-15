#include "SslConnection.hpp"
#include "Logger.hpp"
#include <openssl/err.h>
SslConnection::SslConnection(SSL_CTX* ctx,const std::shared_ptr<TcpConnection>& conn)
                            :ssl_(SSL_new(ctx))
                            ,readBio_(BIO_new(BIO_s_mem()))
                            ,writeBio_(BIO_new(BIO_s_mem()))
                            ,handshakeState_(HandshakeState::HANDSHAKE_IN_PROGRESS)
                            ,tcpConn_(conn)
{
    if(!ssl_)
    {
        LOG_ERROR("SslConnection:   SSL_new   error\n");
        return;
    }
    if(!readBio_||!writeBio_)
    {
        LOG_ERROR("SslConnection:   BIO_new   error\n");
        return;
    }

    SSL_set_bio(ssl_.get(),readBio_,writeBio_);
    SSL_set_accept_state(ssl_.get());

    conn->SetMessageCallback(std::bind(&SslConnection::handleRead,this,std::placeholders::_1,std::placeholders::_2));
}     

void SslConnection::handleHandshake()
{
    int ret=SSL_do_handshake(ssl_.get());
    flushWriteBio();
    if(ret==1)
    {
        handshakeState_=HandshakeState::HANDSHAKE_DONE;
        const char* version=SSL_get_version(ssl_.get());
        const char* cipher=SSL_get_cipher(ssl_.get());
        LOG_INFO("SslConnection:   SSL   handshake   completed\n");
        LOG_INFO("SslConnection:   TLS   VERSION:    %s\n",version);
        LOG_INFO("SslConnection:   TLS   CIPHER:     %s\n",cipher);
    }
    else
    {
        int err=SSL_get_error(ssl_.get(),ret);
        if(err==SSL_ERROR_WANT_READ)
        {
            // Need more data, do nothing for now
            LOG_INFO("SslConnection:   SSL   handshake   in   progress,   need   more   data\n");
        }
        else if(err==SSL_ERROR_WANT_WRITE)
        {
            flushWriteBio();

            LOG_INFO("SslConnection:   SSL   handshake   in   progress,   need   to   write   data\n");
        }
        else
        {
            handshakeState_=HandshakeState::HANDSHAKE_FAILED;
            //LOG_ERROR("SslConnection:   SSL   handshake   failed\n");
            // 获取详细的错误信息

            char errBuf[256];
            unsigned long errCode = ERR_get_error();
            ERR_error_string_n(errCode, errBuf, sizeof(errBuf));
            LOG_ERROR("SslConnection:   SSL   error   details:   %s\n", errBuf);
            //tcpConn_->Shutdown();  // 关闭连接
        }
    }
}

void SslConnection::handleRead(const std::shared_ptr<TcpConnection>& conn,Buffer* buffer)
{
    //LOG_INFO("SslConnection:   received   %d   bytes   encrypted   data\n",buffer->ReadableBytes());
    BIO_write(readBio_,buffer->BeginRead(),buffer->ReadableBytes());
    auto bytes=buffer->ReadableBytes();
    buffer->RetriveAll();
    if(handshakeState_==HandshakeState::HANDSHAKE_IN_PROGRESS)
    {
        handleHandshake();
    }
    else if(handshakeState_==HandshakeState::HANDSHAKE_DONE)
    {
        std::vector<char> readBuffer(bytes);
        int bytesRead=SSL_read(ssl_.get(),readBuffer.data(),readBuffer.size());
        if(bytesRead>0)
        {
            Buffer appBuffer;
            appBuffer.Append(readBuffer.data(),bytesRead);
            if(originalMsgCallback_)
            {
                originalMsgCallback_(conn,&appBuffer);
            }
        }
        else
        {
            int err=SSL_get_error(ssl_.get(),bytesRead);
            LOG_ERROR("SslConnection:   SSL_read   error   %d\n",err);
        }
    }
}

void SslConnection::onEcryption(Buffer* buffer)
{
    SSL_write(ssl_.get(),buffer->BeginRead(),buffer->ReadableBytes());
    buffer->RetriveAll();
    int pending=BIO_pending(writeBio_);
    if(pending<=0)
    {
        LOG_ERROR("SslConnection:   BIO_pending   error\n");
        return;

    }

    std::vector<char> writeBuffer(pending);
    int bytesRead=BIO_read(writeBio_,writeBuffer.data(),pending);
    if(bytesRead<=0)
    {
        LOG_ERROR("SslConnection:   BIO_read   error\n");
        return;
    }

    buffer->Append(writeBuffer.data(),bytesRead);
}

void SslConnection::flushWriteBio()
{
    int pending = BIO_pending(writeBio_);
    while (pending > 0) 
    {
        std::vector<char> buffer(pending);
        int bytesRead = BIO_read(writeBio_, buffer.data(), pending);
        if (bytesRead <= 0) 
        {
            LOG_ERROR("SslConnection:   BIO_read   error\n");
            return;
        }

        auto conn = tcpConn_.lock();
        if (!conn) 
        {
            LOG_ERROR("SslConnection:   TcpConnection   no   longer   exists\n");
            return;
        }
        conn->Send(buffer.data(), bytesRead);

        pending = BIO_pending(writeBio_);
    }
}
