#pragma once
#include "RouterHandler.hpp"
class GomokuServer;
class MenuHandler:public RouterHandler
{
public:
    explicit MenuHandler(GomokuServer* server):server_(server){}
    void handle(const HttpRequest& request,HttpResponse* response) override;
private:
    GomokuServer* server_;
};