#include "GomokuServer.hpp"
#include "EntryHandler.hpp"
#include "SqlPool.hpp"
#include "RegisterHandle.hpp"
#include "LoginHandler.hpp"
#include "MenuHandler.hpp"
#include "GameStartHandler.hpp"
#include "GameMoveHandler.hpp"
#include "GameRestartHandler.hpp"
GomokuServer::GomokuServer()
{
    initialize();
}

void GomokuServer::setThreadNum(int num)
{
    httpServer_.setThreadNum(num);
}

void GomokuServer::initialize()
{
    initializeRoutes();
    initializeDatabase();
    initializeSessionManager();
}

void GomokuServer::start()
{
    httpServer_.start();
}

void GomokuServer::initializeRoutes()
{
    httpServer_.registerGet("/",std::make_shared<EntryHandler>());
    httpServer_.registerGet("/entry",std::make_shared<EntryHandler>());
    httpServer_.registerPost("/register",std::make_shared<RegisterHandle>());
    httpServer_.registerPost("/login",std::make_shared<LoginHandler>(this));
    httpServer_.registerGet("/menu",std::make_shared<MenuHandler>(this));
    httpServer_.registerGet("/aiBot/start",std::make_shared<GameStartHandler>(this));
    httpServer_.registerPost("/aiBot/move",std::make_shared<GameMoveHandler>(this));
    httpServer_.registerPost("/aiBot/restart",std::make_shared<GameRestartHandler>(this));
}

void GomokuServer::initializeDatabase()
{
    SqlPool::getInstance()->init("localhost","wuconan","JAY20020914","Gomoku",3306); 
}

void GomokuServer::initializeSessionManager()
{
    httpServer_.setSessionManager(std::make_unique<HttpSessionManager>());
}
