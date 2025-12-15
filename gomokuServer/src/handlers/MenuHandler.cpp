#include "MenuHandler.hpp"
#include "GomokuServer.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
void MenuHandler::handle(const HttpRequest& request,HttpResponse* response)
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
        LOG_INFO("Unauthorized access to menu.\n");
        return;
    }
    
    std::ifstream file("resource/menu.html",std::ios::in|std::ios::binary);
    if(!file.is_open())
    {
        response->setStatusCode(HttpResponse::StatusCode::InternalServerError); 
        response->setStatusMessage("Internal Server Error");
        response->setBody("Could not load menu page");
        response->setCloseConnection(true);
        LOG_ERROR("Failed to open menu.html\n");
        return;
    }

    file.seekg(0,std::ios::end);
    auto fileSize=file.tellg();
    file.seekg(0,std::ios::beg);

    std::string content;
    content.resize(fileSize);
    file.read(content.data(),fileSize);

    auto pos=content.find("<head>");
    if(pos!=std::string::npos)
    {
        const std::string script="<script>const userId='"+session->getData("userId")+"';</script>";
        content.insert(pos,script);
    }

    response->setStatusCode(HttpResponse::StatusCode::OK);
    response->setStatusMessage("OK");
    response->setContentType("text/html");
    response->setContentLength(content.size());
    response->setBody(content);

}