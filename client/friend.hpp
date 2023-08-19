#pragma once
#include "../src/head.hpp"
using namespace std;

// 查看好友
void CheckFriend(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    msg.task = CHECK_FRIEND;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        int count = 0;
        cout << "   好友列表：" << endl;
        for (int i = 0; i < res.Friends.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "好友名称：" << res.Friends[i] << "  "
                 << "好友ID:  " << res.FriendsID[i]
                 << endl;
        }
        if (res.Friends.empty())
        {
            cout << "您还没有好友！" << endl;
        }
    }
    else
        cout << res.msg << endl;
}

// 申请添加好友
void ApplyAddFriend(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "           添加好友            " << endl;
    cout << "请输入要添加好友的用户ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要添加好友的用户ID:" << endl;
        cin >> id;
    }
    msg.Targetfriendid = id;

    msg.task = APPLY_FRIEND;

    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        cout << "   好友申请发送成功！" << endl;
    }
    else
    {
        cout << res.msg << endl;
        msg.Targetfriendid = 0;
    }
}

// 删除好友
void DelFriend(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    // 展示好友列表
    CheckFriend(sockfd, msg, RecvQue);
    cout << "请输入要删除的好友ID:";
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要删除的好友ID:" << endl;
        cin >> id;
    }
    msg.Targetfriendid = id;

    msg.task = DEL_FRIEND;

    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   删除好友成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 查看在线好友
void OnlineFriend(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    msg.task = ONLINE_FRIEND;

    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   在线好友列表：" << endl;
        int count = 0;
        for (int i = 0; i < res.Friends.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "好友名称：" << res.Friends[i] << "  "
                 << "好友ID:  " << res.FriendsID[i]
                 << endl;
        }
    }
    else
        cout << res.msg << endl;
}

// 查看好友历史消息
void CheckFriendHistory(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    // 展示好友列表
    CheckFriend(sockfd, msg, RecvQue);
    cout << " 请选择查看历史记录的好友ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请选择查看历史记录的好友ID:" << endl;
        cin >> id;
    }
    msg.Targetfriendid = id;

    // 定义任务类型
    msg.task = CHECK_FRIEND_HISTORY;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        queue<chatmsg> chatmsgs;
        // 将vector转化为queue(反向)
        for (auto it = res.chatmsgs.rbegin(); it != res.chatmsgs.rend(); ++it)
        {
            chatmsgs.push(*it);
        }
        cout << "您与该好友的历史记录如下：" << endl;
        // 展示两人历史记录
        cout << "--------------------------------------------------------------" << endl;
        cout << "                   " << msg.Targetfriendid << "                 " << endl;
        cout << "--------------------------------------------------------------" << endl;

        // 加载聊天记录
        while (!chatmsgs.empty())
        {
            struct chatmsg chatmsg = chatmsgs.front();
            chatmsgs.pop();
            cout << chatmsg.speaker << ": " << chatmsg.message << endl
                 << endl;
        }
    }
}
// 屏蔽好友
void BFriend(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请选择要屏蔽的好友ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请选择要屏蔽的好友ID:" << endl;
        cin >> id;
    }
    msg.Targetfriendid = id;

    msg.task = B_FRIEND;

    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        cout << "   屏蔽好友成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 取消屏蔽好友
void DelBFriend(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请选择要取消屏蔽的好友ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请选择要取消屏蔽的好友ID:" << endl;
        cin >> id;
    }
    msg.Targetfriendid = id;

    msg.task = DEL_B_FRIEND;

    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        cout << "   取消屏蔽好友成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 查看屏蔽好友
void CheckBList(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    msg.task = CHECK_BLIST;

    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        int count = 0;
        for (int i = 0; i < res.Friends.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "屏蔽好友名称：" << res.Friends[i] << "  "
                 << "屏蔽好友ID:  " << res.FriendsID[i]
                 << endl;
        }
    }
    else
        cout << res.msg << endl;
}

// 查看好友申请
void CheckFriendApply(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    // 定义任务类型
    msg.task = CHECK_FRIEND_APPLY;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        cout << "   申请列表：" << endl;
        int count = 0;
        for (int i = 0; i < res.Friends.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "用户名称：" << res.Friends[i] << "  "
                 << "用户ID:  " << res.FriendsID[i]
                 << endl;
        }
        cout << "请选择要添加的好友ID:" << endl;
        cin >> msg.Targetfriendid;
        msg.task = ADD_FRIEND;
        // 序列化
        string sendjson = JsonMsg::Json(msg);
        // 发送给服务器进行判定
        IO::SendData(sockfd, sendjson);
        struct respon res = RecvQue.pop();
        if (res.status == SUCCESS)
        {
            cout << "添加好友成功！" << endl;
        }
    }
    else
        cout << res.msg << endl;
}

// 好友聊天
void FriendChat(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int friendid = 0;
    // 展示好友列表
    CheckFriend(sockfd, msg, RecvQue);
    cout << "请选择要聊天的好友ID:" << endl;
    cin >> friendid;
    if (friendid > 500000)
    {
        cout << "输入错误！" << endl;
    }
    else
        msg.Targetfriendid = friendid;

    // 查看历史记录
    msg.task = CHECK_FRIEND_HISTORY;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "--------------------------------------------------------------" << endl;
        cout << "                   " << msg.Targetfriendid << "                 " << endl;
        cout << "--------------------------------------------------------------" << endl;
        queue<chatmsg> chatmsgs;
        // 将vector转化为queue(反向)
        for (auto it = res.chatmsgs.rbegin(); it != res.chatmsgs.rend(); ++it)
        {
            chatmsgs.push(*it);
        }
        // 加载聊天记录
        while (!chatmsgs.empty())
        {
            struct chatmsg chatmsg = chatmsgs.front();
            chatmsgs.pop();
            cout << chatmsg.speaker << ": " << chatmsg.message << endl
                 << endl;
        }
    }
    else
    {
        cout << res.msg << endl;
    }
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do
    {
        cout << "请输入发送的消息：(发送:q退出)" << endl;
        cin.sync();
        getline(cin, msg.chatmessage);
        if (msg.chatmessage == ":q")
            break;
        // 发送消息
        msg.task = FRIEND_CHAT;
        // 序列化
        string sendjson = JsonMsg::Json(msg);
        // 发送给服务器进行判定
        IO::SendData(sockfd, sendjson);
        struct respon res = RecvQue.pop();
        if (res.status == SUCCESS)
            cout << "发送成功！" << endl;
        else
            cout << res.msg << endl;
    } while (1);
}