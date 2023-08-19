#pragma once
#include "../src/head.hpp"

// 提供群名和id

// 创建群聊
void CreatGroup(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int c = 0;
    cout << "请输入要创建的群聊名称:" << endl;
    cin >> msg.Targetgroupname;

    while (1)
    {
        cout << "请输入要组建群聊的成员ID:(输入-1退出)" << endl;
        cin >> c;
        if (c >= 100000 && c <= 500000)
        {
            msg.Targetusers.push_back(c);
        }
        else
            break;
    }

    // 定义任务类型
    msg.task = CREAT_GROUP;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << " 创建群聊成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 查看加入的群聊
bool CheckGroup(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    // 定义任务类型
    msg.task = CHECK_GROUP;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        // 展示加入群聊列表
        int count = 0;
        cout << "--------------------------------------------------------------" << endl;
        cout << "                     "
             << " 群聊列表 "
             << "                    " << endl;
        cout << "--------------------------------------------------------------" << endl;
        for (int i = 0; i < res.Groups.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "群聊名称：" << res.Groups[i] << "  "
                 << "群聊ID:  " << res.GroupsID[i]
                 << endl;
        }
        return true;
    }
    else
    {
        cout << res.msg << endl;
        return false;
    }
}

// 查看群聊历史消息
void CheckGroupHistory(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请选择查看历史记录的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请选择查看历史记录的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请选择查看历史记录的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;

    // 定义任务类型
    msg.task = CHECK_GROUP_HISTORY;
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
        cout << "该群的历史记录如下：" << endl;
        // 展示群聊历史记录
        cout << "--------------------------------------------------------------" << endl;
        cout << "       " << msg.Targetgroupname << "  " << msg.Targetgroupid << "                 " << endl;
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
    else
        cout << res.msg << endl;
}

// 查看群成员
void CheckGroupMem(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请选择查看成员的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请选择查看成员的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请选择查看成员的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;

    // 定义任务类型
    msg.task = CHECK_GROUPMEM;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "群成员如下：" << endl;
        // 展示群成员
        int count = 0;
        for (int i = 0; i < res.Friends.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "成员名称：" << res.Friends[i] << "  "
                 << "成员ID:  " << res.FriendsID[i]
                 << endl;
        }
    }
    else
        cout << res.msg << endl;
}

// 退群
void QuitGroup(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请输入要退出的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请输入要退出的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要退出的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;
    // 定义任务类型
    msg.task = QUIT_GROUP;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   退出群聊成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 添加管理员
void AddAdmin(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int i = 0, c = 0, id;
    cout << "请输入要添加管理员的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请输入要添加管理员的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要添加管理员的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;
    while (1)
    {
        cout << "请输入要添加的管理员ID:(输入-1退出)" << endl;
        cin >> c;
        if (c >= 100000 && c <= 500000)
        {
            msg.Targetusers.push_back(c);
        }
        else
            break;
    }
    // 定义任务类型
    msg.task = ADD_ADMIN;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   添加管理员成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 加入群聊
void JoinGroup(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请输入申请加入的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入申请加入的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;
    // 定义任务类型
    msg.task = JOIN_GROUP;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   申请加入群聊成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 删除管理员
void DelAdmin(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请输入要删除管理员的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请输入要删除管理员的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要删除管理员的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;
    cout << "请输入要删除的管理员ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要删除的管理员ID:" << endl;
        cin >> id;
    }
    msg.Targetfriendid = id;
    // 定义任务类型
    msg.task = DEL_ADMIN;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   删除管理员成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 删除群成员
void DelMem(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请输入要删除成员的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请输入要删除成员的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要删除成员的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;
    cout << "请输入要删除成员的ID:" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要删除成员的ID:" << endl;
        cin >> id;
    }
    msg.Targetfriendid = id;
    // 定义任务类型
    msg.task = DEL_MEM;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   删除群成员成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 解散群
void DelGroup(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请输入要解散的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请输入要解散的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要解散的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;

    // 定义任务类型
    msg.task = DEL_GROUP;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        cout << "   解散群聊成功！" << endl;
    }
    else
        cout << res.msg << endl;
}

// 查看申请列表
void CheckGroupApply(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    cout << "请输入要查看申请的群聊名称:" << endl;
    cin >> msg.Targetgroupname;
    cout << "请输入要查看申请的群聊ID:" << endl;
    cin >> id;
    while (id < 500000 || id > 600000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请输入要查看申请的群聊ID:" << endl;
        cin >> id;
    }
    msg.Targetgroupid = id;

    // 定义任务类型
    msg.task = CHECK_GROUP_APPLY;
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
        cout << "请输入要添加的成员ID: (输入-1退出)" << endl;
        cin >> id;
        if (id == -1)
            return;
        while (id < 100000 || id > 500000)
        {
            cout << "非法输入" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            id = 0;
            cout << "请输入要添加的成员ID: (输入-1退出)" << endl;
            cin >> id;
            if (id == -1)
                return;
        }
        msg.Targetfriendid = id;

        msg.task = ADD_MEM;
        string sendjson = JsonMsg::Json(msg);
        // 发送给服务器进行判定
        IO::SendData(sockfd, sendjson);
        struct respon res = RecvQue.pop();

        if (res.status == SUCCESS)
        {
            cout << "   同意加群成功！" << endl;
        }
        else
            cout << res.msg << endl;
    }
    else
        cout << res.msg << endl;
}

// 查看群聊邀请
void CheckToJoin(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    // 展示请求列表
    msg.task = CHECK_TO_JOIN;
    // 序列化
    string sendjson = JsonMsg::Json(msg);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        int count = 0;
        cout << "   群组列表：" << endl;
        for (int i = 0; i < res.Groups.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "群组名称：" << res.Groups[i] << "  "
                 << "群组ID:  " << res.GroupsID[i]
                 << endl;
        }
        cout << "请选择要加入的群聊ID:" << endl;
        cin >> id;
        while (id < 500000 || id > 600000)
        {
            cout << "非法输入" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            id = 0;
            cout << "请选择要加入的群聊ID:" << endl;
            cin >> id;
        }
        msg.Targetgroupid = id;
        cout << "请选择要加入的群聊名称:" << endl;
        cin >> msg.Targetgroupname;

        msg.task = AGREE_TO_JOIN;
        // 序列化
        string sendjson = JsonMsg::Json(msg);
        // 发送给服务器进行判定
        IO::SendData(sockfd, sendjson);
        struct respon res = RecvQue.pop();
        if (res.status == SUCCESS)
        {
            cout << "加入群聊成功！" << endl;
        }
        else
            cout << res.msg << endl;
    }
    else
        cout << res.msg << endl;
}

// 群聊
void GroupChat(int sockfd, infor &msg, LockedQueue<respon> &RecvQue)
{
    int id;
    // 展示群聊列表
    if (CheckGroup(sockfd, msg, RecvQue))
    {
        cout << "请选择要聊天的群聊ID:" << endl;
        cin >> id;
        while (id < 500000 || id > 600000)
        {
            cout << "非法输入" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            id = 0;
            cout << "请选择要聊天的群聊ID:" << endl;
            cin >> id;
        }
        msg.Targetgroupid = id;
        cout << "请选择要聊天的群聊名称:" << endl;
        cin >> msg.Targetgroupname;
        // 查看历史记录
        msg.task = CHECK_GROUP_HISTORY;
        // 序列化
        string sendjson = JsonMsg::Json(msg);
        // 发送给服务器进行判定
        IO::SendData(sockfd, sendjson);
        struct respon res = RecvQue.pop();
        cout << "--------------------------------------------------------------" << endl;
        cout << "       " << msg.Targetgroupname << "  " << msg.Targetgroupid << "                 " << endl;
        cout << "--------------------------------------------------------------" << endl;

        if (res.status == SUCCESS)
        {
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
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        do
        {
            cout << "请输入发送的消息：(发送:q退出)" << endl;
            cin.sync();
            getline(cin, msg.chatmessage);
            if (msg.chatmessage == ":q")
                break;
            // 发送消息
            msg.task = GROUP_CHAT;
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
}