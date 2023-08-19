#pragma once
#include "../src/head.hpp"
using namespace std;
#include <cassert>
class Epoll
{
public:
    int epollfd;
    struct epoll_event event;
    struct epoll_event events[1024];
    shared_ptr<spdlog::logger> logger;

    Epoll()
    {
    }
    Epoll(int type)
    {
        logger = spdlog::stdout_color_mt("epoll_logger");
        epollfd = epoll_create(3);
        if (epollfd == -1)
        {
            // 处理错误
            logger->error("创建epoll实例失败!");
        }
        event.events = type; // 监听事件类型
    }
    ~Epoll()
    {
        if (close(epollfd) == -1)
        {
            logger->error("关闭epoll实例失败!");
        }
    }

    // 添加指定事件到监听组
    bool AddEvent(int sockfd)
    {
        event.data.fd = sockfd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1)
        {
            logger->error("添加指定事件到监听组失败!");
            return false;
        }
        fcntl(sockfd, F_SETFL, O_NONBLOCK | fcntl(sockfd, F_GETFL));
        return true;
    }

    // 删除事件从监听组
    bool DelEvent(int sockfd)
    {
        event.data.fd = sockfd;
        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, &event) == -1)
        {
            logger->error("从监听组删除指定事件失败!");
            return false;
        }
        return true;
    }
    // client监听可读
    void WaitServerSend(int sockfd, queue<string> &recvjsons)
    {
        int num_events = epoll_wait(epollfd, events, 1024, 5);
        if (num_events == -1 && errno != EINTR)
        {
            logger->error("Epoll监听可读错误!");
            perror("listen:");
            return;
        }
        for (int i = 0; i < num_events; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                // cout << "epoll in" << endl;
                IO::RecvData(sockfd, recvjsons);
                return;
            }
        }
    }
    // server开始监听指定群组
    void EpollListenRead(LockedQueue<int> &ReadQue, int ListenSockfd, Socket &ListenSocket, Database &Database)
    {
        logger->info("Epoll 开始监听!");
        int num_events = epoll_wait(epollfd, events, 1024, -1);
        if (num_events == -1)
        {
            logger->error("Epoll监听可读错误!");
            return;
        }
        logger->info("Epoll监听到了{}个事件!", num_events);
        for (int i = 0; i < num_events; i++)
        {
            // 如果是Listenfd 可读 说明有新连接请求
            if (events[i].events & EPOLLIN && events[i].data.fd == ListenSockfd)
            {
                // 接受请求
                int clientfd = ListenSocket.Accept();
                logger->info("Epoll监听到了新的连接请求__sokcfd:{}", clientfd);
                if (clientfd == -1 || clientfd == 0)
                {
                    logger->error("Epoll接受连接请求失败!");
                }
                // 加入epoll
                this->AddEvent(clientfd);
            }
            // clientfd 挂起
            else if (events[i].events & EPOLLRDHUP && events[i].data.fd != ListenSockfd)
            {
                // 取消监听
                int HupFd = events[i].data.fd;
                logger->info("Epoll检测到__sockfd:{} 挂起", HupFd);
                int id = Database.GetIDBySocket(HupFd);
                Database.ChangeStatus(id, 0);
                Database.DelUserSocket(id);
                // cout << Database.GetIDBySocket(HupFd) << endl;
                this->DelEvent(HupFd);
                close(HupFd);
            }
            // clientfd 有请求消息 加入线程池处理队列
            else if (events[i].events & EPOLLIN && events[i].data.fd != ListenSockfd)
            {
                int ReqFd = events[i].data.fd;
                logger->info("sokcfd:{} 有请求发送", ReqFd);
                ReadQue.push(ReqFd);
            }
        }
    }
};