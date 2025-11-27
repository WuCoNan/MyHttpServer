#pragma once
#include <string>
#include <unordered_map>
class Buffer;
class HttpResponse
{
public:
    enum class StatusCode
    {
        OK = 200,
        BadRequest = 400,
        Unauthorized = 401,
        NotFound = 404,
        Confilict = 409,
        InternalServerError = 500
    };
    void setCloseConnection(bool isClose) { isCloseConnection_ = isClose; }
    bool isCloseConnection() const { return isCloseConnection_; }

    void setVersion(const std::string& version) { version_ = version; }
    const std::string& version() const { return version_; }

    void setStatusCode(StatusCode code) { statusCode_ = code; }
    StatusCode statusCode() const { return statusCode_; }

    void setStatusMessage(const std::string& message) { statusMessage_ = message; }
    const std::string& statusMessage() const { return statusMessage_; }

    void addHeader(const std::string& key, const std::string& value) { headers_[key] = value; }
    void setContentLength(size_t length)
    {
        headers_["Content-Length"] = std::to_string(length);
    }
    void setContentType(const std::string& contentType)
    {
        headers_["Content-Type"] = contentType;
    }
    
    void setBody(const std::string& body) { body_ = body; }
    const std::string& body() const { return body_; }
    void appendToBuffer(Buffer* buffer) const;
private:
    bool isCloseConnection_{false};
    std::string version_;
    StatusCode statusCode_;
    std::string statusMessage_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
};