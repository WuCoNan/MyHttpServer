#include "HttpResponse.hpp"
#include "Buffer.hpp"
#include <sstream>
void HttpResponse::appendToBuffer(Buffer* buffer) const
{
    std::ostringstream oss;
    oss << version_ << " " << static_cast<int>(statusCode_) << " " << statusMessage_ << "\r\n";

    if(isCloseConnection_)
    {
        oss << "Connection: close\r\n";
    }
    else
    {
        oss << "Connection: keep-alive\r\n";
    }

    for (const auto& [key, value] : headers_)
    {
        oss << key << ": " << value << "\r\n";
    }
    oss << "\r\n";
    
    oss<< body_;
    buffer->Append(oss.str().data(), oss.str().size());
}