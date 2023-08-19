#include "../src/head.hpp"
using namespace std;
int main()
{
    spdlog::set_pattern("[%^-%l-%$] [%H:%M:%S][thread %t][%n] %v");
    spdlog::set_level(spdlog::level::debug);
    // 初始化服务器
    Server server;
    // 初始化 Epoll
    Epoll epoll(EPOLLIN | EPOLLRDHUP | EPOLLET);
    // 初始化消息队列
    LockedQueue<int> ReadQue;
    // 初始化数据库
    Database DataBase{};
    // 初始化线程池 将处理好的信息放入消息队列
    Logic logic(&ReadQue, server.LSock.sockfd, &DataBase, &epoll);
    // 准备接受请求
    server.Run(epoll, ReadQue,DataBase);
    // 监听clientfd 有可读加入消息队列
}
