#pragma once
#include "../src/head.hpp"
using namespace std;

class IO
{
public:
    static void SendData(int sockfd, string &str)
    {
        if (!isSocketClosed(sockfd))
        {
            int len = str.length();
            char *data = (char *)malloc(len + 4);
            int bigLen = htonl(len);
            memcpy(data, &bigLen, 4);
            memcpy(data + 4, str.c_str(), len);
            ssize_t DataNum = send(sockfd, data, len + 4, 0);
            // cout << "send datasize" << DataNum << endl;
            if (DataNum == -1)
            {
                perror("Send data error");
                return;
            }
            free(data);
        }
    }
    // 客户端接收数据
    static bool RecvData(int sockfd, queue<string> &strings)
    {
        string str;
        char buff[MAX_SIZE];
        while (1)
        {
            int size = 0;
            ssize_t n = recv(sockfd, &size, 4, 0);
            if (n <= 0)
            {
                return false;
            }
            size = ntohl(size);
            ssize_t num = recv(sockfd, buff, size, 0);
            if (num <= 0)
            {
                return false;
            }
            buff[num] = '\0';
            str = string(buff);
            // cout << "recv: " << str << endl;
            strings.push(str);
        }
        return true;
    }
    static bool isSocketClosed(int sockfd)
    {
        int error;
        socklen_t len = sizeof(error);
        int result = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
        if (result < 0)
        {
            return true;
        }
        return false;
    }
    static void flush_input_buffer()
    {
        std::cin.clear();                                         // 清除错误标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略输入缓冲区中的所有字符，直到遇到换行符为止
    }
};
