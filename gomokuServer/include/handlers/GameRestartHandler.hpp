#pragma once

#include "RouterHandler.hpp"
class GomokuServer;

class GameRestartHandler:public RouterHandler
{
public:
    explicit GameRestartHandler(GomokuServer* server) : server_(server) {}
    void handle(const HttpRequest& request, HttpResponse* response) override;
private:
    GomokuServer* server_;
};