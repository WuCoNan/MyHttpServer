#pragma once

#include "RouterHandler.hpp"

class GomokuServer;
class GameMoveHandler : public RouterHandler
{
public:
    explicit GameMoveHandler(GomokuServer* server);
    void handle(const HttpRequest& request, HttpResponse* response) override;
private:
    GomokuServer* server_;
};