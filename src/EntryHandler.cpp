#include "EntryHandler.hpp"
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Logger.hpp"
#include <fstream>
void EntryHandler::handle(const HttpRequest& request, HttpResponse* response)
{
    std::ifstream file("resource/entry.html",std::ios::in | std::ios::binary);
    if(!file.is_open())
    {
        response->setVersion(request.version());
        response->setStatusCode(HttpResponse::StatusCode::NotFound);
        response->setStatusMessage("Not Found");
        response->setBody("404 Not Found");
        LOG_ERROR("EntryHandler:   file   not   found\n");
        return;
    }

    file.seekg(0,std::ios::end);
    auto fileSize=file.tellg();
    file.seekg(0,std::ios::beg);

    std::string content;
    content.resize(fileSize);
    file.read(content.data(),fileSize);

    response->setVersion(request.version());
    response->setStatusCode(HttpResponse::StatusCode::OK);
    response->setStatusMessage("OK");
    response->setContentType("text/html");
    response->setContentLength(content.size());
    response->setBody(content);
}