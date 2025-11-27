#pragma once
#include "RouterHandler.hpp"
#include <string>
class RegisterHandle : public RouterHandler
{
public:
    void handle(const HttpRequest& request, HttpResponse* response) override;
private:
    bool isExitUser(const std::string& username);
    int insertUser(const std::string& username, const std::string& password);
};