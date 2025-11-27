#pragma once
#include "HttpServer.hpp"
#include "unordered_map"
#include <memory>
#include <mutex>
class AiGame;
class GomokuServer
{
public:
    GomokuServer();
    void start();
    void setThreadNum(int num);
    HttpSessionManager* getSessionManager() const
    {
        return httpServer_.getSessionManager();
    }
    std::unordered_map<std::string,std::shared_ptr<AiGame>> activeGames_;
    std::mutex mutexForGames_;
private:
    void initialize();
    void initializeRoutes();
    void initializeDatabase();
    void initializeSessionManager();
    HttpServer httpServer_;
};