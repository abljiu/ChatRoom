#pragma once
#include "../src/head.hpp"
using namespace std;

class Client
{
private:
    string ip;
    int port;
    int sockfd;
    int flag = 0;

public:
    Client(string ip, uint16_t port, int sockfd)
    {
        this->ip = ip;
        this->port = port;
        this->sockfd = sockfd;
    }
    ~Client()
    {
    }
    void Run()
    {
        // 初始化接收队列
        LockedQueue<respon> RecvQue;
        // 初始化 Epoll
        Epoll epoll(EPOLLIN | EPOLLET);
        epoll.AddEvent(sockfd);
        //开始计时
        chrono::steady_clock::time_point start = chrono::steady_clock::now();
        // 初始化recv线程
        thread RecvThread = thread(recvfunc, sockfd, &RecvQue, &epoll, &flag, &start);

        infor infor;
        menu(infor, sockfd, ip, port, RecvQue, epoll,&start);
        flag = 1;
        if (RecvThread.joinable())
            RecvThread.join();
    }

    static void recvfunc(int sockfd, LockedQueue<respon> *RecvQue, Epoll *epoll, int *flag, chrono::steady_clock::time_point *start)
    {
        while (1)
        {
            // 接收服务器返回的数据
            queue<string> recvjsons;
            while (recvjsons.empty())
            {
                // epoll监听 可读
                epoll->WaitServerSend(sockfd, recvjsons);
                if (*flag == 1)
                    return;
            }
            while (!recvjsons.empty())
            {
                string recvjson = recvjsons.front();
                recvjsons.pop();
                // 反序列化
                struct respon res;
                JsonMsg::UnJson(recvjson, res);
                if (res.status == NOTICE)
                    cout << "\t\t\t||新通知|| " << res.msg << endl;
                else if (res.status == SUCCESS || res.status == FAILED)
                {
                    // cout << "PUSH" << endl;
                    RecvQue->push(res);
                }
                else if (res.status == HEART_BEAT)
                {
                    // 刷新时间
                    *start = chrono::steady_clock::now();
                }
            }
        }
    }
}

;