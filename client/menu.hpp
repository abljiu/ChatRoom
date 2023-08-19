#pragma once
#include "../src/head.hpp"
using namespace std;

void firstmenu()
{
    cout << "————————————————欢迎来到聊天室！———————————————" << endl;
    cout << "| 1.登录：                         2.注册：     | " << endl;
    cout << "| 3.忘记密码（找回）：             4.注销：     | " << endl;
    cout << "|                     5.退出：                  | " << endl;
    cout << "———————————————————————————————————————" << endl;
}

void Login(infor &infor, int sockfd, string ip, int port, LockedQueue<respon> &RecvQue, Epoll &epoll, chrono::steady_clock::time_point *start)
{
    int id;
    cout << "           登录界面           " << endl;
    cout << "请输入你的ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入你的ID:" << endl;
        cin >> id;
    }
    infor.id = id;
    system("stty -echo");
    cout << "请输入你的密码：" << endl;
    cin >> infor.password;
    system("stty echo");
    infor.task = LOGIN;

    // 序列化
    string sendjson = JsonMsg::Json(infor);
    // 摘树
    epoll.DelEvent(sockfd);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    cout << "发送请求" << endl;
    // 挂树
    epoll.AddEvent(sockfd);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        cout << "   登录成功！" << endl;
        infor.name = res.name;
        InsideMenu(sockfd, infor, RecvQue, epoll, start);
    }
    else
    {
        cout << res.msg << endl;
    }
}

void Register(infor &infor, int sockfd, LockedQueue<respon> &RecvQue)
{
    int ok = 0;
    string password1;
    string password2;
    cout << "           注册界面           " << endl;
    cout << "请输入你的昵称：" << endl;
    cin >> infor.name;
    system("stty -echo");
    while (ok == 0)
    {
        cout << "请输入你的密码：" << endl;
        cin >> password1;
        cout << "请确认你的密码：" << endl;
        cin >> password2;
        if (password1 == password2)
        {
            infor.password = password1;
            ok = 1;
        }
        else
            cout << "   两次输入的密码不一致！" << endl;
    }
    system("stty echo");
    cout << "请输入你的密保：" << endl;
    cin >> infor.propass;

    infor.task = REGISTER;

    // 序列化
    string sendjson = JsonMsg::Json(infor);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);

    //  反序列化
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        system("clear");
        cout << res.msg << endl;
    }
    else
    {
        cout << res.msg << endl;
    }
}

void Logout(infor &infor, int sockfd, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "           注销界面           " << endl;
    cout << "请输入你的ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入你的ID:" << endl;
        cin >> id;
    }
    infor.id = id;
    cout << "请输入你的密码：" << endl;
    cin >> infor.password;

    infor.task = LOGOUT;

    // 序列化
    string sendjson = JsonMsg::Json(infor);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);

    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   注销成功！" << endl;
    }
    else
        cout << res.msg << endl;
    cout << res.msg << endl;
}

void Quit(int sockfd) // 退出
{
    // close(sockfd);
    cout << "   欢迎下次使用" << endl;
}

void Findpassword(infor &infor, int sockfd, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "           找回密码界面           " << endl;
    cout << "请输入你ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入你的ID:" << endl;
        cin >> id;
    }
    infor.id = id;
    cout << "请输入你的密保：" << endl;
    cin >> infor.propass;

    infor.task = FINDPASSWORD;
    // 序列化
    string sendjson = JsonMsg::Json(infor);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);

    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "您的密码是：" << res.msg << endl;
    }
    else
        cout << res.msg << endl;
}

void menu(infor &infor, int sockfd, string ip, int port, LockedQueue<respon> &RecvQue, Epoll &epoll, chrono::steady_clock::time_point *start)
{
    int choice;
    while (1)
    {
        firstmenu();
        cin >> choice;
        if (choice <= 5 && choice > 0)
        {
            switch (choice - 5)
            {
            case LOGIN:
                Login(infor, sockfd, ip, port, RecvQue, epoll, start);
                break;
            case REGISTER:
                Register(infor, sockfd, RecvQue);
                break;
            case LOGOUT:
                Logout(infor, sockfd, RecvQue);
                break;
            case QUIT:
                Quit(sockfd);
                return;
            case FINDPASSWORD:
                Findpassword(infor, sockfd, RecvQue);
                break;
            }
        }
        else
        {
            cout << "请输入正确的选项！" << endl;
            IO::flush_input_buffer();
        }
    }
}
