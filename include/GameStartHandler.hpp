#pragma once
#include "RouterHandler.hpp"

class GomokuServer;
class GameStartHandler:public RouterHandler
{
public:
    explicit GameStartHandler(GomokuServer* server):server_(server){}
    void handle(const HttpRequest& request,HttpResponse* response) override;
private:
    GomokuServer* server_;
};