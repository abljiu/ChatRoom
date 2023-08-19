#pragma once
#include "head.hpp"
using namespace std;

class Socket
{

public:
    int sockfd;
    shared_ptr<spdlog::logger> logger;
    Socket()
    {
        logger = spdlog::stdout_color_mt("socket_logger");
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            logger->error("socket 创建失败!");
            exit(1);
        }
        int reuse = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        {
            logger->error("setsockopt");
            // Handle error
        }
        //  cout << "sockfd " << sockfd << endl;
    }

    ~Socket()
    {
        close(sockfd);
        // cout << "socket close " << sockfd << endl;
    }

    void Listen(int backlog)
    {
        if (listen(sockfd, backlog) < 0)
        {
            logger->error("listen 监听失败!");
            exit(1);
        }
    }

    void Bind(int port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logger->error("bind 绑定失败!");
            exit(1);
        }
    }

    int Accept()
    {
        struct sockaddr_in peer;
        memset(&peer, 0, sizeof(peer));
        socklen_t len = sizeof(peer);
        int clientfd = accept(sockfd, (struct sockaddr *)&peer, &len);
        if (clientfd < 0)
        {
            logger->error("accept 接受请求失败!");
            exit(1);
        }
        return clientfd;
    }

    void Connect(string ip, int port)
    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            logger->error("connect 连接失败!");
            exit(1);
        }
    }
};
