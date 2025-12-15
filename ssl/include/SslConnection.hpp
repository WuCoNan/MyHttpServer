#pragma once
#include <openssl/ssl.h>
#include <memory>
#include "TcpConnection.hpp"
class SslConnection
{
public:
    SslConnection(SSL_CTX* ctx,const std::shared_ptr<TcpConnection>& conn);
    void setOriginalMessageCallback(const MessageCallback& cb)
    {
        originalMsgCallback_=cb;
    }
    void onEcryption(Buffer* buffer);
private:
    void handleRead(const std::shared_ptr<TcpConnection>& conn,Buffer* buffer);
    void handleHandshake();
    void flushWriteBio();
    enum class HandshakeState
    {
        HANDSHAKE_IN_PROGRESS,
        HANDSHAKE_DONE,
        HANDSHAKE_FAILED
    };
    struct SslDeleter
    {
        void operator()(SSL* ssl) const
        {
            if(ssl)
            {
                SSL_shutdown(ssl);
                SSL_free(ssl);
            }
        }
    };
    std::unique_ptr<SSL, SslDeleter> ssl_;
    BIO* readBio_;
    BIO* writeBio_;
    HandshakeState handshakeState_{HandshakeState::HANDSHAKE_IN_PROGRESS};
    MessageCallback originalMsgCallback_;
    std::weak_ptr<TcpConnection> tcpConn_;
};