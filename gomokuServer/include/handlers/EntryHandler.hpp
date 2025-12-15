#pragma once
#include "RouterHandler.hpp"

class EntryHandler : public RouterHandler
{
public:
    void handle(const HttpRequest& request, HttpResponse* response) override;
};