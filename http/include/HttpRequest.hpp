#pragma once
#include <string>
#include <unordered_map>
class HttpRequest
{
public:
    enum class Method
    {
        GET,
        POST,
        HEAD,
        PUT,
        DELETE,
        INVALID
    };
    size_t contentLength() const { return content_length_; }
    void setContentLength(size_t length) { content_length_ = length; }

    Method method() const { return method_; }
    bool setMethod(const char *start, const char *end);

    void setPath(const char *begin, const char *end) { path_ = std::string(begin, end); }
    const std::string &path() const { return path_; }

    void setQueryParams(const char *start, const char *end);
    const std::unordered_map<std::string, std::string> &queryParams() const { return queryParams_; }

    void setPathParams(const std::string& key, const std::string& value)
    {
        pathParams_[key]=value;
    }
    bool setVersion(const char *start, const char *end);
    const std::string &version() const { return version_; }

    void addHeader(const std::string &key, const std::string &value) { headers_[key] = value; }
    const std::unordered_map<std::string, std::string> &headers() const { return headers_; }
    const std::string &getHeader(const std::string &key) const
    {
        auto it = headers_.find(key);
        if (it != headers_.end())
        {
            return it->second;
        }
        static const std::string empty;
        return empty;
    }

    std::string getSessionId() const;
    
    void setBody(const char *start, const char *end) { body_ = std::string(start, end); }
    const std::string &body() const { return body_; }

    
private:
    Method method_;
    std::string version_;
    std::string path_;
    std::unordered_map<std::string, std::string> pathParams_;
    std::unordered_map<std::string, std::string> queryParams_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
    size_t content_length_{0};

};