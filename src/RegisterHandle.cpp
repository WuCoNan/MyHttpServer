#include "RegisterHandle.hpp"
#include "SqlPool.hpp"

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <nlohmann/json.hpp>
void RegisterHandle::handle(const HttpRequest& request, HttpResponse* response)
{
    // Handle the registration logic here
    nlohmann::json bodyJson=nlohmann::json::parse(request.body());
    const std::string username=bodyJson["username"];
    const std::string password=bodyJson["password"];
    if(!isExitUser(username))
    {
        int userId=insertUser(username,password);
        nlohmann::json resJson;
        resJson["status"]="success";
        resJson["message"]="Register successful";
        resJson["userId"]=userId;

        response->setVersion("HTTP/1.1");
        response->setStatusCode(HttpResponse::StatusCode::OK);
        response->setStatusMessage("OK");
        response->setContentType("application/json");
        response->setContentLength(resJson.dump().size());
        response->setBody(resJson.dump());
    }
    else
    {
        nlohmann::json resJson;
        resJson["status"]="error";
        resJson["message"]="Username already exists";

        response->setVersion("HTTP/1.1");
        response->setStatusCode(HttpResponse::StatusCode::Confilict);
        response->setStatusMessage("Conflict");
        response->setContentType("application/json");
        response->setContentLength(resJson.dump().size());
        response->setBody(resJson.dump());
    }
}

bool RegisterHandle::isExitUser(const std::string& username)
{
    // Check if the user already exists in the database
    auto sqlPool=SqlPool::getInstance();
    std::string query="SELECT COUNT(*) AS user_count FROM users WHERE username='"+username+"';";

    auto queryResult=sqlPool->executeQuery(query);
    if(queryResult["user_count"][0]=="0")
    {
        return false; // User does not exist
    }
    return true; // User exists
}

int RegisterHandle::insertUser(const std::string& username, const std::string& password)
{
    // Insert a new user into the database
    auto sqlPool=SqlPool::getInstance();
    std::string query="INSERT INTO users (username, password) VALUES ('"+username+"', '"+password+"');";

    sqlPool->execUpdate(query);

    query="SELECT id FROM users WHERE username='"+username+"';";
    auto queryResult=sqlPool->executeQuery(query);
    return std::stoi(queryResult["id"][0]);
}