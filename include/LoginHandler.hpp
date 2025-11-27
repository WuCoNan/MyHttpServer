#pragma once
#include "RouterHandler.hpp"
#include <string>
class GomokuServer;
class LoginHandler : public RouterHandler
{
public:
    explicit LoginHandler(GomokuServer* server) : server_(server) {}
    void handle(const HttpRequest& request,HttpResponse* response) override;
private:
    int isValidUser(const std::string& username,const std::string& password);
    GomokuServer* server_;
};