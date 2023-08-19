#pragma once
#include "../src/head.hpp"
using namespace std;
void cmap(infor &infor)
{
    cout << "------------------欢迎来到聊天室！-----------------" << endl;
    cout << "_______________________私聊_________________________" << endl;
    cout << "ID:  " << infor.id << "   NAME:  " << infor.name << endl;
    cout << "1.选择好友进行聊天         2.查看在线好友          " << endl;
    cout << "3.查看好友申请             4.查看历史记录          " << endl;
    cout << "5.删除好友                 6.添加好友              " << endl;
    cout << "7.查看好友                 8.屏蔽好友              " << endl;
    cout << "9.取消屏蔽好友             10.查看屏蔽好友         " << endl;
    cout << "                  11.退出登录                      " << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "_______________________群聊_______________________" << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "12.查看申请加群消息            13.查看加入群聊          " << endl;
    cout << "14.群聊                        15.查看历史记录           " << endl;
    cout << "16.查看群聊成员                17.解散群聊（群主）      " << endl;
    cout << "18.删除群管理员（群主）        19.添加群管理员（群主）   " << endl;
    cout << "20.删除普通成员（群主和管理）  21.查看群聊邀请           " << endl;
    cout << "22.申请加群                    23.创建群聊               " << endl;
    cout << "                   24.退出群聊                      " << endl;
    cout << "-------------------------------------------------" << endl;
    cout << "_______________________其它_______________________" << endl;
    cout << "--------------------------------------------------" << endl;
    cout << "25.发送文件                    26.接收文件             " << endl;
}

// 退出登录
void LogExit(int sockfd, infor &infor, LockedQueue<respon> &RecvQue, Epoll &epoll)
{
    // 定义任务类型
    infor.task = LOGEXIT;
    // 序列化
    string sendjson = JsonMsg::Json(infor);
    // 摘树
    epoll.DelEvent(sockfd);
    // 发送给服务器进行判定
    IO::SendData(sockfd, sendjson);

    // 挂树
    epoll.AddEvent(sockfd);
    cout << "发送成功！" << endl;
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        cout << "   退出成功！" << endl;
    }
    else
        cout << "   请重试！" << endl;
}

// 发送文件
void SendFile(int sockfd, infor &infor, LockedQueue<respon> &RecvQue)
{
    int id;
    string path;
    cout << "请选择发送的好友id" << endl;
    cin >> id;
    while (id < 100000 || id > 500000)
    {
        cout << "非法输入" << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        id = 0;
        cout << "请选择发送的好友id:" << endl;
        cin >> id;
    }
    infor.Targetfriendid = id;
    cout << "请输入要发送的文件路径：" << endl;
    cin >> path;

    size_t pos = path.rfind('/');

    if (pos != string::npos)
    {
        infor.filename = path.substr(pos + 1);
    }

    struct stat st;
    if (stat(path.c_str(), &st) != 0)
    {
        cout << "文件不存在！" << endl;
        return;
    }
    infor.bytes = st.st_size;

    int fileFd = open(path.c_str(), O_RDONLY);
    if (fileFd < 0)
    {
        cout << "打开文件失败!" << endl;
        return;
    }

    // 定义任务类型
    infor.task = SEND_FILE;
    // 序列化
    string sendjson = JsonMsg::Json(infor);
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();

    if (res.status == SUCCESS)
    {
        int count = 0;
        off_t offset = 0;
        ssize_t sentBytes;
        while (offset < infor.bytes)
        {
            sentBytes = sendfile(sockfd, fileFd, &offset, infor.bytes - offset);
            // this_thread::sleep_for(chrono::seconds(1));
            if (sentBytes == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    continue;
                }
                else
                {
                    cout << "发送文件失败" << endl;
                    break;
                }
            }
            else if (sentBytes == 0)
            {
                cout << "文件发送完成" << endl;
                break;
            }
        }
        cout << "文件发送完成" << std::endl;
    }
    else
        cout << res.msg << endl;
    close(fileFd);
}

// 接收文件
void RecvFile(int sockfd, infor &infor, LockedQueue<respon> &RecvQue, Epoll &epoll)
{
    // 定义任务类型
    infor.task = FILES;
    // 序列化
    string sendjson = JsonMsg::Json(infor);
    IO::SendData(sockfd, sendjson);
    struct respon res = RecvQue.pop();
    if (res.status == SUCCESS)
    {
        int count = 0;
        cout << "   文件列表：" << endl;
        for (int i = 0; i < res.Friends.size(); i++)
        {
            cout << ++count
                 << ".  "
                 << "文件名称：" << res.Friends[i] << endl;
        }
        if (res.Friends.empty())
        {
            cout << "您还没有文件！" << endl;
            return;
        }
    }
    else
    {
        cout << "查看失败！" << endl;
        return;
    }

    cout << "请选择接收的文件名( 输入-1退出)" << endl;
    cin >> infor.filename;
    if (infor.filename == "-1")
        return;
    while (find(res.Friends.begin(), res.Friends.end(), infor.filename) == res.Friends.end())
    {
        cout << "输入错误！" << endl;
        cout << "请选择接收的文件名( 输入-1退出)" << endl;
        cin >> infor.filename;
        if (infor.filename == "-1")
            return;
    }
    // 定义任务类型
    infor.task = RECV_FILE;
    // 序列化
    sendjson = JsonMsg::Json(infor);
    epoll.DelEvent(sockfd);

    IO::SendData(sockfd, sendjson);
    queue<string> recvstring;
    while (recvstring.empty())
    {
        IO::RecvData(sockfd, recvstring);
    }

    string recvjson = recvstring.front();
    recvstring.pop();
    JsonMsg::UnJson(recvjson, res);
    if (res.status == SUCCESS)
    {
        string filename = "../" + infor.filename;
        int fd = open(filename.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        if (fd < 0)
        {
            perror("open");
            return;
        }
        char buf[MAX_SIZE];
        int count = 0;
        while (1)
        {
            int ret = recv(sockfd, buf, MAX_SIZE - 1, 0);
            cout << res.bytes << endl;
            if (ret <= 0 && count == res.bytes)
            {
                break;
            }
            else if (ret > 0)
            {
                count += ret;
                ssize_t total_bytes_written = 0;
                while (total_bytes_written < ret)
                {
                    ssize_t bytes_written = write(fd, buf + total_bytes_written, ret - total_bytes_written);
                    if (bytes_written < 0)
                    {
                        perror("write");
                        return;
                    }
                    total_bytes_written += bytes_written;
                }
            }
        }
    }
    else
    {
        cout << "接收文件失败！" << endl;
    }

    epoll.AddEvent(sockfd);
}

// 发送心跳
static void HeartBeat(int sockfd, infor *heart, atomic<bool> *ShouldExit, mutex *mtx, condition_variable *cv)
{
    unique_lock<std::mutex> lock(*mtx);
    while (!ShouldExit->load())
    {
        heart->task = HEART_BEAT;
        string jsonstring = JsonMsg::Json(*heart);
        IO::SendData(sockfd, jsonstring);
        cv->wait_for(lock, chrono::seconds(30), [ShouldExit]
                     { return ShouldExit->load(); });
    }
}
// 检测心跳
static void CheckHeart(atomic<bool> *ShouldExit, chrono::steady_clock::time_point *start, mutex *mtx, condition_variable *cv)
{
    while (!ShouldExit->load())
    {
        unique_lock<std::mutex> lock(*mtx);
        cv->wait_for(lock, chrono::seconds(40), [ShouldExit]
                     { return ShouldExit->load(); });
        chrono::steady_clock::time_point end = chrono::steady_clock::now();
        // if ((end - *start) > chrono::seconds(50))
        // {
        //     cout << "服务器离线！" << endl;
        //     return;
        // }
    }
}

void InsideMenu(int sockfd, infor &infor, LockedQueue<respon> &RecvQue, Epoll &epoll, chrono::steady_clock::time_point *start)
{
    mutex mtx;
    condition_variable cv;
    // atomic<bool> ShouldExit = false;

    // 初始化心跳
    // thread HeartThread = thread(HeartBeat, sockfd, &infor, &ShouldExit, &mtx, &cv);
    // 检测服务器心跳
    // thread CheckHeartThread = thread(CheckHeart, &ShouldExit, start, &mtx, &cv);

    int choice = 0;
    while (choice != LOGEXIT)
    {
        cmap(infor);
        cin >> choice;
        if (choice >= 1 && choice <= 26)
        {
            switch (choice)
            {
            case LOGEXIT:
                system("clear");
                LogExit(sockfd, infor, RecvQue, epoll);
                // ShouldExit.store(true);
                // cv.notify_all();
                // if (HeartThread.joinable())
                //     HeartThread.join();
                // if (CheckHeartThread.joinable())
                //     CheckHeartThread.join();
                break;
            case CHECK_FRIEND:
                system("clear");
                CheckFriend(sockfd, infor, RecvQue);
                break;
            case APPLY_FRIEND:
                system("clear");
                ApplyAddFriend(sockfd, infor, RecvQue);
                break;
            case DEL_FRIEND:
                system("clear");
                DelFriend(sockfd, infor, RecvQue);
                break;
            case FRIEND_CHAT:
                system("clear");
                FriendChat(sockfd, infor, RecvQue);
                break;
            case ONLINE_FRIEND:
                system("clear");
                OnlineFriend(sockfd, infor, RecvQue);
                break;
            case CHECK_FRIEND_HISTORY:
                system("clear");
                CheckFriendHistory(sockfd, infor, RecvQue);
                break;
            case CHECK_GROUP_HISTORY:
                system("clear");
                CheckGroupHistory(sockfd, infor, RecvQue);
                break;
            case B_FRIEND:
                system("clear");
                BFriend(sockfd, infor, RecvQue);
                break;
            case DEL_B_FRIEND:
                system("clear");
                DelBFriend(sockfd, infor, RecvQue);
                break;
            case CREAT_GROUP:
                system("clear");
                CreatGroup(sockfd, infor, RecvQue);
                break;
            case QUIT_GROUP:
                system("clear");
                QuitGroup(sockfd, infor, RecvQue);
                break;
            case CHECK_GROUP:
                system("clear");
                CheckGroup(sockfd, infor, RecvQue);
                break;
            case ADD_ADMIN:
                system("clear");
                AddAdmin(sockfd, infor, RecvQue);
                break;
            case DEL_GROUP:
                system("clear");
                DelGroup(sockfd, infor, RecvQue);
                break;
            case GROUP_CHAT:
                system("clear");
                GroupChat(sockfd, infor, RecvQue);
                break;
            case DEL_ADMIN:
                system("clear");
                DelAdmin(sockfd, infor, RecvQue);
                break;
            case CHECK_GROUPMEM:
                system("clear");
                CheckGroupMem(sockfd, infor, RecvQue);
                break;
            case DEL_MEM:
                system("clear");
                DelMem(sockfd, infor, RecvQue);
                break;
            case JOIN_GROUP:
                system("clear");
                JoinGroup(sockfd, infor, RecvQue);
                break;
            case CHECK_TO_JOIN:
                system("clear");
                CheckToJoin(sockfd, infor, RecvQue);
                break;
            case CHECK_FRIEND_APPLY:
                system("clear");
                CheckFriendApply(sockfd, infor, RecvQue);
                break;
            case CHECK_GROUP_APPLY:
                system("clear");
                CheckGroupApply(sockfd, infor, RecvQue);
                break;
            case SEND_FILE:
                system("clear");
                SendFile(sockfd, infor, RecvQue);
                break;
            case RECV_FILE:
                system("clear");
                RecvFile(sockfd, infor, RecvQue, epoll);
                break;
            case CHECK_BLIST:
                system("clear");
                CheckBList(sockfd, infor, RecvQue);
                break;
            }
        }
        else
        {
            cout << "选项有误，请重新选择！" << endl;
            IO::flush_input_buffer();
        }
    }
}
