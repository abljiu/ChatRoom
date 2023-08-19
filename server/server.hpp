#pragma once
#include "../src/head.hpp"
using namespace std;

class Server
{
public:
    Socket LSock;
    shared_ptr<spdlog::logger> logger;
    Server()
    {
        logger = spdlog::stdout_color_mt("server_logger");
        logger->info("服务器初始化!");
        LSock.Bind(7975);
        LSock.Listen(5);
        int optval = 1;
        if (setsockopt(LSock.sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
        {
            logger->error("设置套接字选项失败");
            exit(1);
        }
        int tcp_keepidle = 60;  // 60秒没有数据交换后开始发送Keep-Alive探测
        int tcp_keepintvl = 10; // 每10秒发送一次探测
        int tcp_keepcnt = 5;    // 如果没有响应，发送5次探测后关闭连接

        if (setsockopt(LSock.sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &tcp_keepidle, sizeof(tcp_keepidle)) == -1)
        {
            perror("设置TCP_KEEPIDLE选项失败");
            exit(1);
        }
    }

    ~Server()
    {
    }

    // 将accept的fd加入epoll
    void Run(Epoll &epoll, LockedQueue<int> &FdQue, Database &Database)
    {
        logger->info("服务器启动！");
        epoll.AddEvent(LSock.sockfd);
        // 循环监听
        while (1)
        {
            epoll.EpollListenRead(FdQue, LSock.sockfd, LSock, Database);
        }
    }
};
