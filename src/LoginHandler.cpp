#include "LoginHandler.hpp"
#include "GomokuServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "SqlPool.hpp"
#include <nlohmann/json.hpp>
void LoginHandler::handle(const HttpRequest& request,HttpResponse* response)
{
    // Here would be the logic to handle login requests.
    // This is just a placeholder implementation.
    if(request.getHeader("Content-Type")!="application/json"||request.body().empty())
    {
        response->setStatusCode(HttpResponse::StatusCode::BadRequest);
        response->setStatusMessage("Bad Request");
        response->setBody("Invalid request");
        response->setCloseConnection(true);
        return;
    }

    nlohmann::json bodyJson=nlohmann::json::parse(request.body());
    const std::string username=bodyJson["username"];
    const std::string password=bodyJson["password"];

    auto userId=isValidUser(username,password);
    if(userId!=-1)
    {
        auto session=server_->getSessionManager()->getSession(request,response);
        session->setData("username",username);
        session->setData("userId",std::to_string(userId));
        session->setData("isLoggedIn", "true");

        nlohmann::json resJson;
        resJson["success"]=true;
        resJson["userId"]=userId;
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::OK);
        response->setStatusMessage("OK");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("User %s logged in successfully.\n",username.c_str());
    }
    else
    {
        nlohmann::json resJson;
        resJson["status"]="error";
        resJson["message"]="Invalid username or password";
        auto resBody=resJson.dump(4);

        response->setStatusCode(HttpResponse::StatusCode::Unauthorized);
        response->setStatusMessage("Unauthorized");
        response->setContentType("application/json");
        response->setContentLength(resBody.size());
        response->setBody(resBody);
        LOG_INFO("Failed login attempt for username: %s\n",username.c_str());
    }
     
}
int LoginHandler::isValidUser(const std::string& username,const std::string& password)
{
    auto sqlPool=SqlPool::getInstance();
    const std::string query="SELECT id FROM users WHERE username='"+username+"' AND password='"+password+"';";
    auto queryResult=sqlPool->executeQuery(query);
    if(!queryResult["id"].empty())
    {
        return std::stoi(queryResult["id"][0]);
    }
    return -1;
}