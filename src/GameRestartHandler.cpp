#include "GameRestartHandler.hpp"
#include "GomokuServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "AiGame.hpp"
#include <nlohmann/json.hpp>

void GameRestartHandler::handle(const HttpRequest& request,HttpResponse* response)
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
        LOG_INFO("Unauthorized access to restart game.\n");
        return;
    }

    std::string userId=session->getData("userId");
    server_->activeGames_[userId]=std::make_shared<AiGame>(userId);

    nlohmann::json resJson;
    resJson["status"]="ok";
    auto resBody=resJson.dump(4);

    response->setStatusCode(HttpResponse::StatusCode::OK);
    response->setStatusMessage("OK");
    response->setContentType("application/json");
    response->setContentLength(resBody.size());
    response->setBody(resBody);
    LOG_INFO("Game restarted for user %s against AI.\n",userId.c_str());
}