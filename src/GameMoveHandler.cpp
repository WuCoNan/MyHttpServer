#include "GameMoveHandler.hpp"
#include "GomokuServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "AiGame.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
GameMoveHandler::GameMoveHandler(GomokuServer* server)
    : server_(server)
{
}

void GameMoveHandler::handle(const HttpRequest& request,HttpResponse* response)
{
    auto session = server_->getSessionManager()->getSession(request,response);
    bool isLoggedIn = session->getData("isLoggedIn") == "true";
    if (!isLoggedIn)
    {
        nlohmann::json resJson;
        resJson["status"]="error";
        resJson["message"]="User not logged in";
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::Unauthorized);
        response->setStatusMessage("Unauthorized");
        response->setCloseConnection(true);
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("Unauthorized access to make move.\n");
        return;
    }
    
    if(request.getHeader("Content-Type") != "application/json")
    {
        nlohmann::json resJson;
        resJson["status"]="error";
        resJson["message"]="Invalid Content-Type";
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::BadRequest);
        response->setStatusMessage("Bad Request");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("Invalid Content-Type for move request.\n");
        return;
    }

    std::string userId = session->getData("userId");
    if(!server_->activeGames_[userId])
    {
        server_->activeGames_[userId] = std::make_shared<AiGame>(userId);
    }

    nlohmann::json reqJson=nlohmann::json::parse(request.body());
    int x=reqJson["x"],y=reqJson["y"];
    auto game=server_->activeGames_[userId];

    if(!game->playerMove(x,y))
    {
        nlohmann::json resJson;
        resJson["status"]="error";
        resJson["message"]="Invalid move";
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::BadRequest);
        response->setStatusMessage("Bad Request");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("Invalid move attempted by user %s.\n",userId.c_str());
        return;
    }
    if(game->isPlayerWinner())
    {
        nlohmann::json resJson;
        resJson["status"]="ok";
        resJson["winner"]="human";
        resJson["next_turn"]="none";
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::OK);
        response->setStatusMessage("OK");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("User %s wins the game.\n",userId.c_str());
        return;
    }
    if(game->isDraw())
    {
        nlohmann::json resJson;
        resJson["status"]="ok";
        resJson["winner"]="draw";
        resJson["next_turn"]="none";
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::OK);
        response->setStatusMessage("OK");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("Game ended in a draw for user %s.\n",userId.c_str());
        return;
    }

    game->aiMove();
    auto [aiX,aiY]=game->getLastAiMove();
    if(game->isAiWinner())
    {
        nlohmann::json resJson;
        resJson["status"]="ok";
        resJson["winner"]="ai";
        resJson["next_turn"]="none";
        resJson["last_move"]={{"x",aiX},{"y",aiY}};
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::OK);
        response->setStatusMessage("OK");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("AI wins the game against user %s.\n",userId.c_str());
        return;
    }
    if(game->isDraw())
    {
        nlohmann::json resJson;
        resJson["status"]="ok";
        resJson["winner"]="draw";
        resJson["next_turn"]="none";
        auto resBody=resJson.dump(4);
        response->setStatusCode(HttpResponse::StatusCode::OK);
        response->setStatusMessage("OK");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("Game ended in a draw for user %s.\n",userId.c_str());
        return;
    }

    
    nlohmann::json resJson;
    resJson["status"]="ok";
    resJson["winner"]="none";
    resJson["next_turn"]="human";
    resJson["last_move"]={{"x",aiX},{"y",aiY}};
    auto resBody=resJson.dump(4);
    response->setStatusCode(HttpResponse::StatusCode::OK);
    response->setStatusMessage("OK");
    response->setContentType("application/json");
    response->setContentLength(resBody.size());
    response->setBody(resBody);
    LOG_INFO("AI made a move for user %s: (%d, %d).\n", userId.c_str(), aiX, aiY);
}