#include <Client.hpp>
#include <EventLoop.hpp>

#define THREADCOUNT 3
#define SESSIONCOUNT 100

int main()
{
    EventLoop mainLoop;
    Client echoClient(&mainLoop,THREADCOUNT,SESSIONCOUNT);
    mainLoop.Loop();
    return 0;
}