#include "GameStartHandler.hpp"
#include "GomokuServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
void GameStartHandler::handle(const HttpRequest& request,HttpResponse* response)
{
    auto session=server_->getSessionManager()->getSession(request,response);
    bool isLoggedIn=session->getData("isLoggedIn")=="true";
    if(!isLoggedIn)
    {
        nlohmann::json resJson;
        resJson["status"]="error";
        resJson["message"]="User not logged in";
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::Unauthorized);
        response->setStatusMessage("Unauthorized");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("Unauthorized access to start game.\n");
        return;
    }

    std::ifstream file("resource/ChessGameVsAi.html",std::ios::in|std::ios::binary);
    if(!file.is_open())
    {
        response->setStatusCode(HttpResponse::StatusCode::NotFound);
        response->setStatusMessage("Not Found");
        response->setCloseConnection(true);
        return;
    }

    file.seekg(0,std::ios::end);
    auto fileSize=file.tellg();
    file.seekg(0,std::ios::beg);
    std::string content;
    content.resize(fileSize);
    file.read(content.data(),fileSize);

    response->setStatusCode(HttpResponse::StatusCode::OK);
    response->setStatusMessage("OK");
    response->setContentType("text/html");
    response->setContentLength(content.size());
    response->setBody(content);
}