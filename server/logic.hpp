#pragma once
#include "../src/database.hpp"
class Logic
{
private:
    vector<thread> threads;
    thread HeartCheck;

public:
    Logic(LockedQueue<int> *FdQue, int sockfd, Database *DataBase, Epoll *epoll)
    {
        threads.resize(THREAD_POLL_NUM);
        for (int i = 0; i < THREAD_POLL_NUM; ++i)
        {
            threads[i] = thread(threadFunction, FdQue, sockfd, DataBase, epoll);
        }
        // HeartCheck = thread(CheckHeart, DataBase);
    }
    ~Logic()
    {
        for (int i = 0; i < THREAD_POLL_NUM; ++i)
        {
            threads[i].join();
        }
    }
    // 线程池中线程处理请求
    static void threadFunction(LockedQueue<int> *ReadQue, int sockfd, Database *DataBase, Epoll *epoll)
    {
        auto logger = spdlog::get("server_logger");
        while (1)
        {
            infor infor;
            infor.sockfd = ReadQue->pop();
            respon res;

            // 摘树
            epoll->DelEvent(infor.sockfd);
            // 接收数据
            queue<string> recvjsons;
            IO::RecvData(infor.sockfd, recvjsons);
            logger->info("成功接收数据");
            while (!recvjsons.empty())
            {
                string recvjson = recvjsons.front();
                recvjsons.pop();
                // 反序列化
                JsonMsg::UnJson(recvjson, infor);
                DealInfor(infor, *DataBase, res, sockfd, *epoll);
                logger->info("处理完成");
                // 序列化
                string jsonstr = JsonMsg::Json(res);
                // 发送给客户端
                IO::SendData(infor.sockfd, jsonstr);
                if (infor.task != SEND_FILE && infor.task != RECV_FILE)
                {
                    // 挂树
                    epoll->AddEvent(infor.sockfd);
                }
                logger->info("成功发送");
            }
        }
    }

    // 处理拿到的信息
    static void DealInfor(infor &infor, Database &Database, respon &res, int sockfd, Epoll &epoll)
    {
        auto logger = spdlog::get("server_logger");
        switch (infor.task)
        {
        case LOGIN:
            logger->info("处理登录逻辑");
            res = Login(infor, Database);
            break;
        case REGISTER:
            logger->info("处理注册逻辑");
            res = Register(infor, Database);
            break;
        case LOGOUT:
            logger->info("处理注销逻辑");
            res = Logout(infor, Database);
            break;
        case FINDPASSWORD:
            logger->info("处理找回密码逻辑");
            res = FindPassWord(infor, Database);
            break;
        case ONLINE_FRIEND:
            logger->info("处理查看在线好友逻辑");
            res = OnlineFriend(infor, Database);
            break;
        case FRIEND_CHAT:
            logger->info("处理私聊逻辑");
            res = FriendChat(infor, Database);
            break;
        case CHECK_FRIEND_HISTORY:
            logger->info("处理查看好友历史记录逻辑");
            res = CheckFriendHistory(infor, Database);
            break;
        case DEL_FRIEND:
            logger->info("处理删除好友逻辑");
            res = DelFriend(infor, Database);
            break;
        case APPLY_FRIEND:
            logger->info("处理申请添加好友逻辑");
            res = ApplyAddFriend(infor, Database);
            break;
        case ADD_FRIEND:
            logger->info("处理同意好友申请逻辑");
            res = AddFriend(infor, Database);
            break;
        case CHECK_FRIEND:
            logger->info("处理查看好友逻辑");
            res = CheckFriend(infor, Database);
            break;
        case B_FRIEND:
            logger->info("处理查看屏蔽好友逻辑");
            res = BFriend(infor, Database);
            break;
        case DEL_B_FRIEND:
            logger->info("处理删除屏蔽好友逻辑");
            res = DelBFriend(infor, Database);
            break;
        case CHECK_BLIST:
            logger->info("处理登录逻辑");
            res = CheckBList(infor, Database);
            break;
        case CHECK_FRIEND_APPLY:
            logger->info("处理查看好友申请逻辑");
            res = CheckFriendApply(infor, Database);
            break;
        case LOGEXIT:
            logger->info("处理退出登录逻辑");
            res = LogExit(infor, Database);
            break;
        case CHECK_GROUP_APPLY:
            logger->info("处理查看加群申请逻辑");
            res = CheckGroupApply(infor, Database);
            break;
        case CHECK_GROUP:
            logger->info("处理查看已加入群聊逻辑");
            res = CheckGroup(infor, Database);
            break;
        case GROUP_CHAT:
            logger->info("处理群聊逻辑");
            res = GroupChat(infor, Database);
            break;
        case CHECK_GROUP_HISTORY:
            logger->info("处理查看群聊历史记录逻辑");
            res = CheckGroupHistory(infor, Database);
            break;
        case CHECK_GROUPMEM:
            logger->info("处理查看群组成员逻辑");
            res = CheckGroupMem(infor, Database);
            break;
        case DEL_GROUP:
            logger->info("处理解散群聊逻辑");
            res = DelGroup(infor, Database);
            break;
        case DEL_ADMIN:
            logger->info("处理删除管理逻辑");
            res = DelAdmin(infor, Database);
            break;
        case ADD_ADMIN:
            logger->info("处理添加管理逻辑");
            res = AddAdmin(infor, Database);
            break;
        case DEL_MEM:
            logger->info("处理删除群成员逻辑");
            res = DelMem(infor, Database);
            break;
        case CHECK_TO_JOIN:
            logger->info("处理查看群邀请逻辑");
            res = CheckToJoin(infor, Database);
            break;
        case JOIN_GROUP:
            logger->info("处理申请加入群聊逻辑");
            res = JoinGroup(infor, Database);
            break;
        case CREAT_GROUP:
            logger->info("处理创建群聊逻辑");
            res = CreatGroup(infor, Database);
            break;
        case QUIT_GROUP:
            logger->info("处理退出逻辑");
            res = QuitGroup(infor, Database);
            break;
        case SEND_FILE:
            logger->info("处理发送文件逻辑");
            res = SendFile(infor, Database, epoll);
            break;
        case RECV_FILE:
            logger->info("处理接受文件逻辑");
            res = RecvFile(infor, Database, epoll);
            break;
        case FILES:
            logger->info("处理查看文件逻辑");
            res = Files(infor, Database);
            break;
        case ADD_MEM:
            logger->info("处理同意加入群逻辑");
            res = AddMem(infor, Database);
            break;
        case AGREE_TO_JOIN:
            logger->info("处理接受群聊邀请逻辑");
            res = AgreeToJoin(infor, Database);
            break;
        case HEART_BEAT:
            logger->info("接收到客户端心跳");
            res = HeartBeat(infor, Database);
            break;
        default:
            break;
        }
    }

    // 登录逻辑
    static respon Login(infor &infor, Database &Database)
    {
        queue<string> sendmsg;
        respon respon;
        struct respon notice;
        respon.status = FAILED;
        if (Database.GetPassword(infor.id) == infor.password && !Database.UserIsOnline(infor.id))
        {
            if (Database.ChangeStatus(infor.id, ONLINE) && Database.FindUser(infor.id))
            {
                infor.name = Database.GerUserName(infor.id);
                Database.SetUserSocket(infor.id, infor.sockfd);
                respon.status = SUCCESS;
                respon.name = Database.GerUserName(infor.id);
                respon.msg = "登录成功！";
                // 有待发送消息
                if (Database.GetUnSendMsg(infor.id, sendmsg))
                {
                    cout << "有待发消息！" << endl;
                    while (!sendmsg.empty())
                    {
                        notice.status = NOTICE;
                        notice.msg = sendmsg.front();
                        sendmsg.pop();
                        string jsonrespon = JsonMsg::Json(notice);
                        IO::SendData(Database.GetUserSocket(infor.id), jsonrespon);
                    }
                }
            }
            else
                respon.msg = "登录失败！";
        }
        else
            respon.msg = "用户在线或密码错误！";
        return respon;
    }

    // 注册逻辑
    static respon Register(infor &infor, Database &Database)
    {
        int newUID;
        respon respon;
        respon.status = FAILED;
        if (Database.FindUser(infor.id))
        {
            respon.msg = "用户已经存在！";
        }
        Database.AddUser(infor.name, infor.password, infor.propass, newUID);
        respon.status = SUCCESS;
        respon.msg = "注册成功! 您的ID: " + to_string(newUID);

        return respon;
    }

    // 注销逻辑
    static respon Logout(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        if (!Database.FindUser(infor.id))
        {
            respon.msg = "用户不存在！";
        }
        else if (Database.DelUser(infor.id))
        {
            respon.status = SUCCESS;
            respon.msg = "注销成功！";
        }
        else
            respon.msg = "注销失败!";

        return respon;
    }

    // 找回密码逻辑
    static respon FindPassWord(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        if (Database.FindUser(infor.id))
        {
            if (Database.GetPropass(infor.id) != infor.propass)
            {
                respon.msg = Database.GetPassword(infor.id);
                respon.status = SUCCESS;
            }
            else
                respon.msg = "密保不正确！";
        }
        else
            respon.msg = "用户不存在！";

        return respon;
    }

    // 查看在线好友
    static respon OnlineFriend(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        vector<string> OnlineFriends;
        vector<int> OnlineFriendsID;

        vector<string> Friends;
        vector<int> FriendsID;

        vector<string> OnlineUsers;
        vector<int> OnlineUsersID;

        OnlineUsers = Database.GetOlineUser();
        Friends = Database.GetFriends(infor.id);
        OnlineUsersID = Database.GetOlineUserID();
        FriendsID = Database.GetFriendsID(infor.id);

        // 排序
        sort(OnlineUsers.begin(), OnlineUsers.end());
        sort(Friends.begin(), Friends.end());

        sort(OnlineFriendsID.begin(), OnlineFriendsID.end());
        sort(OnlineUsersID.begin(), OnlineUsersID.end());
        // 求交集
        set_intersection(
            OnlineUsers.begin(), OnlineUsers.end(),
            Friends.begin(), Friends.end(),
            back_inserter(OnlineFriends));

        set_intersection(
            OnlineUsersID.begin(), OnlineUsersID.end(),
            FriendsID.begin(), FriendsID.end(),
            back_inserter(OnlineFriendsID));
        respon.Friends = OnlineFriends;
        respon.FriendsID = OnlineFriendsID;

        respon.status = SUCCESS;
        return respon;
    }

    // 与好友进行聊天
    static respon FriendChat(infor &infor, Database &Database)
    {
        vector<int> friends;
        vector<int> userBfriends = Database.GetBFriendsID(infor.id);
        vector<int> friBfriends = Database.GetBFriendsID(infor.Targetfriendid);
        respon respon;
        struct respon notice;
        respon.status = FAILED;
        friends = Database.GetFriendsID(infor.id);
        // 好友存在
        if (Database.FindUser(infor.Targetfriendid) && (find(friends.begin(), friends.end(), infor.Targetfriendid) != friends.end()))
        {
            for (const auto &uBf : userBfriends)
            {
                if (uBf == infor.Targetfriendid)
                {
                    respon.msg = "该好友被屏蔽！";
                    return respon;
                }
            }
            for (const auto &fBf : friBfriends)
            {
                if (fBf == infor.id)
                {
                    respon.msg = "您被该好友屏蔽！";
                    return respon;
                }
            }
            // 数据库添加聊天记录
            chatmsg chatmsg;
            chatmsg.message = infor.chatmessage;
            chatmsg.speaker = infor.name;
            string jsonchatmsg = JsonMsg::Json(chatmsg);
            Database.AddChatRecord(infor.id, infor.Targetfriendid, jsonchatmsg);
            // cout << "数据存储成功！" << endl;
            // 将数据转发给用户
            if (Database.UserIsOnline(infor.Targetfriendid))
            {
                notice.status = NOTICE;
                notice.msg = "私聊: " + infor.name + ": " + infor.chatmessage;
                string jsonrespon = JsonMsg::Json(notice);
                IO::SendData(Database.GetUserSocket(infor.Targetfriendid), jsonrespon);
                respon.status = SUCCESS;
                respon.msg = "发送成功！";
            }
            else // 用户不在线
            {
                // 将消息加入待发送组
                respon.status = SUCCESS;
                Database.AddUnSendMsg(infor.Targetfriendid, "私聊: " + infor.name + ": " + infor.chatmessage);
                respon.msg = "发送成功！";
                // cout << "加入待发群组" << endl;
            }
        }
        else
        {
            // cout << "好友不存在" << endl;
            respon.msg = "好友不存在！";
        }
        return respon;
    }

    // 查看历史记录
    static respon CheckFriendHistory(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (!Database.GetChatRecords(infor.id, infor.Targetfriendid).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.chatmsgs = Database.GetChatRecords(infor.id, infor.Targetfriendid);
        }
        respon.msg = "历史记录为空！";

        return respon;
    }

    // 同意好友申请
    static respon AddFriend(infor &infor, Database &Database)
    {
        respon respon;
        struct respon notice;
        respon.status = FAILED;

        if (Database.AddFriend(infor.id, infor.Targetfriendid) && Database.AddFriend(infor.Targetfriendid, infor.id) && Database.DelAddFriend(infor.id, infor.Targetfriendid))
        {
            // 将数据转发给用户
            if (Database.UserIsOnline(infor.Targetfriendid))
            {
                notice.status = NOTICE;
                notice.msg = "好友申请成功: " + to_string(infor.id);
                string jsonrespon = JsonMsg::Json(notice);
                IO::SendData(Database.GetUserSocket(infor.Targetfriendid), jsonrespon);
            }
            else // 用户不在线
            {
                // 将消息加入待发送组
                Database.AddUnSendMsg(infor.Targetfriendid, "好友申请成功: " + to_string(infor.id));
                respon.msg = "发送成功！";
                // cout << "加入待发群组" << endl;
            }
            respon.status = SUCCESS;
            respon.msg = "添加成功！";
        }
        respon.msg = "添加失败！";
        return respon;
    }

    // 删除好友
    static respon DelFriend(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        vector<int> friends = Database.GetFriendsID(infor.id);
        // 好友存在
        if (Database.FindUser(infor.Targetfriendid) && (find(friends.begin(), friends.end(), infor.Targetfriendid) != friends.end()))
        {
            if (Database.DelFriend(infor.id, infor.Targetfriendid))
            {
                respon.status = SUCCESS;
                respon.msg = "删除成功！";
            }
        }
        else
        {
            respon.msg = "好友不存在，删除失败！";
        }
        return respon;
    }

    // 发送好友申请
    static respon ApplyAddFriend(infor &infor, Database &Database)
    {
        respon notice;
        respon respon;
        respon.status = FAILED;
        vector<int> friends = Database.GetFriendsID(infor.id);
        // 不在自己好友列表
        if (find(friends.begin(), friends.end(), infor.Targetfriendid) == friends.end())
        {
            // 用户存在
            if (Database.FindUser(infor.Targetfriendid))
            {
                if (Database.ApplyAddFriend(infor.Targetfriendid, infor.id))
                {
                    // 将数据转发给用户
                    if (Database.UserIsOnline(infor.Targetfriendid))
                    {
                        notice.status = NOTICE;
                        notice.msg = "好友申请: " + to_string(infor.id);
                        string jsonrespon = JsonMsg::Json(notice);
                        IO::SendData(Database.GetUserSocket(infor.Targetfriendid), jsonrespon);
                    }
                    else // 用户不在线
                    {
                        // 将消息加入待发送组
                        Database.AddUnSendMsg(infor.Targetfriendid, "好友申请: " + to_string(infor.id));
                        respon.msg = "发送成功！";
                        // cout << "加入待发群组" << endl;
                    }
                    respon.status = SUCCESS;
                    respon.msg = "申请成功！";
                }
                else
                {
                    respon.msg = "申请失败！";
                }
            }
            else
            {
                respon.msg = "用户不存在,申请失败！";
            }
        }
        else
        {
            respon.msg = "用户已经是好友,申请失败！";
        }
        return respon;
    }

    // 查看好友
    static respon CheckFriend(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (!Database.GetFriends(infor.id).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.Friends = Database.GetFriends(infor.id);
            respon.FriendsID = Database.GetFriendsID(infor.id);
        }
        respon.msg = "好友列表为空！";

        return respon;
    }

    // 屏蔽好友
    static respon BFriend(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        vector<int> friends = Database.GetFriendsID(infor.id);
        // 好友存在
        if (Database.FindUser(infor.Targetfriendid) && (find(friends.begin(), friends.end(), infor.Targetfriendid) != friends.end()))
        {
            if (Database.AddBFriend(infor.id, infor.Targetfriendid))
            {
                respon.status = SUCCESS;
                respon.msg = "屏蔽成功！";
            }
            respon.msg = "屏蔽失败！";
        }
        else
        {
            respon.msg = "好友不存在,屏蔽失败！";
        }

        return respon;
    }

    // 删除屏蔽好友
    static respon DelBFriend(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        vector<int> friends = Database.GetBFriendsID(infor.id);
        // 屏蔽好友存在
        if (Database.FindUser(infor.Targetfriendid) && (find(friends.begin(), friends.end(), infor.Targetfriendid) != friends.end()))
        {
            if (Database.DelBFriend(infor.id, infor.Targetfriendid))
            {
                respon.status = SUCCESS;
                respon.msg = "删除屏蔽成功！";
            }
            respon.msg = "删除屏蔽失败！";
        }
        else
        {
            respon.msg = "好友未屏蔽,删除屏蔽失败！";
        }

        return respon;
    }

    // 查看屏蔽好友
    static respon CheckBList(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (!Database.GetBFriends(infor.id).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.Friends = Database.GetBFriends(infor.id);
            respon.FriendsID = Database.GetBFriendsID(infor.id);
        }
        respon.msg = "好友列表为空！";
        return respon;
    }

    // 查看好友申请
    static respon CheckFriendApply(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (!Database.GetFriendsApply(infor.id).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.Friends = Database.GetFriendsApply(infor.id);
            respon.FriendsID = Database.GetFriendsApplyID(infor.id);
        }
        respon.msg = "好友申请列表为空！";
        return respon;
    }

    // 退出登录
    static respon LogExit(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (Database.ChangeStatus(infor.id, OFFLINE) && Database.DelUserSocket(infor.id))
        {
            // cout << "退出成功！" << endl;
            respon.status = SUCCESS;
            respon.msg = "退出成功！";
        }
        return respon;
    }

    // 查看申请加群消息
    static respon CheckGroupApply(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        vector<int> admins = Database.GetGroupAdminsID(infor.Targetgroupid);
        if (Database.GetGroupMasterID(infor.Targetgroupid) != infor.id && !(find(admins.begin(), admins.end(), infor.id) != admins.end()))
        {
            respon.msg = "您没有权力查看";
            return respon;
        }
        if (!Database.GetGroupsApply(infor.Targetgroupid).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.Friends = Database.GetGroupsApply(infor.Targetgroupid);
            respon.FriendsID = Database.GetGroupsApplyID(infor.Targetgroupid);
        }
        respon.msg = "群聊申请列表为空！";
        return respon;
    }

    // 查看加入群聊
    static respon CheckGroup(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (!Database.GetGroupIn(infor.id).empty() && !Database.GetGroupInID(infor.id).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.Groups = Database.GetGroupIn(infor.id);
            respon.GroupsID = Database.GetGroupInID(infor.id);
        }
        respon.msg = "群聊列表为空！";
        return respon;
    }

    // 群聊
    static respon GroupChat(infor &infor, Database &Database)
    {
        vector<int> GroupMem = Database.GetGroupMemID(infor.Targetgroupid);
        respon respon;
        struct respon notice;
        respon.status = FAILED;
        // 用户在群组
        if ((find(GroupMem.begin(), GroupMem.end(), infor.id) != GroupMem.end()))
        {
            // 数据库添加聊天记录
            chatmsg chatmsg;
            chatmsg.message = infor.chatmessage;
            chatmsg.speaker = infor.name;
            string jsonchatmsg = JsonMsg::Json(chatmsg);
            Database.AddGroupRecord(infor.Targetgroupid, jsonchatmsg);
            // 数据打包
            notice.status = NOTICE;
            notice.msg = infor.Targetgroupname + "_群聊: " + infor.name + ": " + infor.chatmessage;
            string jsonrespon = JsonMsg::Json(notice);
            // 将数据转发给群组用户
            for (const auto &mem : GroupMem)
            {
                if (mem != infor.id)
                {
                    if (Database.UserIsOnline(mem))
                    {
                        IO::SendData(Database.GetUserSocket(mem), jsonrespon);
                        respon.status = SUCCESS;
                        respon.msg = "发送成功！";
                    }
                    else // 用户不在线
                    {
                        // 将消息加入待发送组

                        respon.status = SUCCESS;
                        Database.AddUnSendMsg(mem, infor.Targetgroupname + "_群聊: " + infor.name + ": " + infor.chatmessage);
                        respon.msg = "发送成功！";
                    }
                }
            }
        }
        else
        {
            respon.status = FAILED;
            respon.msg = "群聊不存在！";
        }
        return respon;
    }

    // 查看群聊历史记录
    static respon CheckGroupHistory(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (!Database.GetGroupRecords(infor.Targetgroupid).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.chatmsgs = Database.GetGroupRecords(infor.Targetgroupid);
        }
        respon.msg = "历史记录为空！";
        return respon;
    }

    // 查看群聊成员
    static respon CheckGroupMem(infor &infor, Database &Database)
    {
        vector<int> GroupMem = Database.GetGroupMemID(infor.Targetgroupid);
        respon respon;
        respon.status = FAILED;
        if ((find(GroupMem.begin(), GroupMem.end(), infor.id) != GroupMem.end()))
        {
            if (!Database.GetGroupMem(infor.Targetgroupid).empty())
            {
                respon.status = SUCCESS;
                respon.msg = "查看成功！";
                respon.Friends = Database.GetGroupMem(infor.Targetgroupid);
                respon.FriendsID = Database.GetGroupMemID(infor.Targetgroupid);
            }
            else
            {
                respon.msg = "群成员为空！";
            }
        }
        else
        {
            respon.msg = "您不在该群组！";
        }

        return respon;
    }

    // 解散群聊（群主）
    static respon DelGroup(infor &infor, Database &Database)
    {
        respon respon;
        struct respon notice;
        respon.status = FAILED;
        vector<int> groupmems = Database.GetGroupMemID(infor.Targetgroupid);
        if (Database.GetGroupMasterID(infor.Targetgroupid) != infor.id)
        {
            respon.msg = "您没有权力解散";
            return respon;
        }
        if (Database.GetGroupMasterID(infor.Targetgroupid) == infor.id)
        {
            if (Database.DelGroup(infor.Targetgroupid))
            {
                notice.status = NOTICE;
                notice.msg = infor.Targetgroupname + "_群聊解散";
                string jsonrespon = JsonMsg::Json(notice);
                for (const auto &mem : groupmems)
                {
                    Database.DelFromGroup(mem, infor.Targetgroupid);
                    if (Database.UserIsOnline(mem))
                    {
                        IO::SendData(Database.GetUserSocket(mem), jsonrespon);
                        respon.status = SUCCESS;
                        respon.msg = "发送成功！";
                    }
                    else // 用户不在线
                    {
                        // 将消息加入待发送组
                        respon.status = SUCCESS;
                        Database.AddUnSendMsg(mem, infor.Targetgroupname + "_群聊解散");
                        respon.msg = "发送成功！";
                    }
                }
                respon.status = SUCCESS;
                respon.msg = "删除成功！";
            }
            else
            {
                respon.msg = "删除失败！";
            }
        }
        else
        {
            respon.msg = "您没有权限！";
        }
        return respon;
    }

    // 删除群管理员（群主）
    static respon DelAdmin(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        if (Database.GetGroupMasterID(infor.Targetgroupid) == infor.id)
        {
            if (Database.DelGroupAdmin(infor.Targetgroupid, infor.Targetfriendid))
            {
                respon.status = SUCCESS;
                respon.msg = "删除成功！";
            }
            else
            {
                respon.msg = "删除失败！";
            }
        }
        else
        {
            respon.msg = "您没有权限！";
        }
        return respon;
    }

    // 添加群管理员（群主）
    static respon AddAdmin(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (Database.GetGroupMasterID(infor.Targetgroupid) == infor.id)
        {
            if (Database.AddAdmins(infor.Targetgroupid, infor.Targetusers))
            {
                respon.status = SUCCESS;
                respon.msg = "添加成功！";
            }
            else
            {
                respon.msg = "添加失败！";
            }
        }
        else
        {
            respon.msg = "您没有权限！";
        }
        return respon;
    }

    // 删除普通成员（群主和管理）
    static respon DelMem(infor &infor, Database &Database)
    {
        vector<int> admins = Database.GetGroupAdminsID(infor.Targetgroupid);
        respon respon;
        respon.status = FAILED;
        // 踢出的是普通成员
        if (infor.Targetfriendid != Database.GetGroupMasterID(infor.Targetgroupid) && (find(admins.begin(), admins.end(), infor.Targetfriendid) == admins.end()))
        {
            if (Database.GetGroupMasterID(infor.Targetgroupid) == infor.id || (find(admins.begin(), admins.end(), infor.id) != admins.end()))
            {
                if (Database.DelFromGroup(infor.Targetfriendid, infor.Targetgroupid))
                {
                    respon.status = SUCCESS;
                    respon.msg = "添加成功！";
                }
                else
                {
                    respon.msg = "添加失败！";
                }
            }
            else
            {
                respon.msg = "您没有权限！";
            }
        }
        else
        {
            respon.msg = "您无法踢出该成员！";
        }

        return respon;
    }

    // 同意加入群聊（群主和管理）
    static respon AddMem(infor &infor, Database &Database)
    {
        vector<int> admins = Database.GetGroupAdminsID(infor.Targetgroupid);
        respon respon;
        respon.status = FAILED;

        if (Database.GetGroupMasterID(infor.Targetgroupid) == infor.id || (find(admins.begin(), admins.end(), infor.id) != admins.end()))
        {
            if (Database.AddToGroup(infor.Targetfriendid, infor.Targetgroupid) && Database.DelAddGroup(infor.Targetgroupid, infor.Targetfriendid))
            {
                respon.status = SUCCESS;
                respon.msg = "添加成功！";
            }
            else
            {
                respon.msg = "添加失败！";
            }
        }
        else
        {
            respon.msg = "您没有权限！";
        }
        return respon;
    }

    // 查看群聊邀请
    static respon CheckToJoin(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        if (!Database.CheckToJoin(infor.id).empty())
        {
            respon.status = SUCCESS;
            respon.msg = "查看成功！";
            respon.Groups = Database.CheckToJoin(infor.id);
            respon.GroupsID = Database.CheckToJoinID(infor.id);
        }
        respon.msg = "群聊邀请列表为空！";
        return respon;
    }

    // 同意群聊邀请
    static respon AgreeToJoin(infor &infor, Database &Database)
    {
        respon respon;
        struct respon notice;
        respon.status = FAILED;
        vector<int> tojoingroups = Database.CheckToJoinID(infor.id);

        // 在待加入群组中
        if (find(tojoingroups.begin(), tojoingroups.end(), infor.Targetgroupid) != tojoingroups.end())
        {
            // 加入成功
            if (Database.AddToGroup(infor.id, infor.Targetgroupid) && Database.DelToJoin(infor.id, infor.Targetgroupid))
            {
                int master = Database.GetGroupMasterID(infor.Targetgroupid);

                // 将数据转发给群主或管理员
                if (Database.UserIsOnline(master))
                {
                    notice.status = NOTICE;
                    notice.msg = "接受群聊申请: " + Database.GerUserName(infor.id);
                    string jsonrespon = JsonMsg::Json(notice);
                    IO::SendData(Database.GetUserSocket(master), jsonrespon);
                }
                else // 用户不在线
                {
                    // 将消息加入待发送组
                    respon.status = SUCCESS;
                    Database.AddUnSendMsg(master, "接受群聊申请: " + Database.GerUserName(infor.id));
                    // cout << "加入待发群组" << endl;
                }
                respon.status = SUCCESS;
                respon.msg = "加入成功！";
            }
            else
            {
                respon.msg = "加入失败！";
            }
        }
        else
        {
            respon.msg = "群组不存在！";
        }

        return respon;
    }

    // 申请加群
    static respon JoinGroup(infor &infor, Database &Database)
    {
        struct respon respon;
        struct respon notice;
        respon.status = FAILED;
        infor.Targetgroupname = Database.GerGroupName(infor.Targetgroupid);
        if (Database.ExistsGroup(infor.Targetgroupid))
        {
            if (Database.ApplyAddGroup(infor.Targetgroupid, infor.id))
            {
                int master = Database.GetGroupMasterID(infor.Targetgroupid);
                // 将数据转发给群主或管理员
                if (Database.UserIsOnline(master))
                {
                    notice.status = NOTICE;
                    notice.msg = infor.Targetgroupname + "群聊申请: " + to_string(infor.id);
                    string jsonrespon = JsonMsg::Json(notice);
                    IO::SendData(Database.GetUserSocket(master), jsonrespon);
                }
                else // 用户不在线
                {
                    // 将消息加入待发送组
                    respon.status = SUCCESS;
                    Database.AddUnSendMsg(master, infor.Targetgroupname + "群聊申请: " + to_string(infor.id));
                    // cout << "加入待发群组" << endl;
                }
                vector<int> admins;
                admins = Database.GetGroupAdminsID(infor.Targetgroupid);
                for (auto &admin : admins)
                {
                    if (Database.UserIsOnline(admin))
                    {
                        notice.status = NOTICE;
                        notice.msg = infor.Targetgroupname + "群聊申请: " + to_string(infor.id);
                        string jsonrespon = JsonMsg::Json(notice);
                        IO::SendData(Database.GetUserSocket(admin), jsonrespon);
                    }
                    else // 用户不在线
                    {
                        // 将消息加入待发送组
                        respon.status = SUCCESS;
                        Database.AddUnSendMsg(admin, infor.Targetgroupname + "群聊申请: " + to_string(infor.id));
                        // cout << "加入待发群组" << endl;
                    }
                }
                respon.status = SUCCESS;
                respon.msg = "申请成功！";
            }
        }
        else
        {
            respon.msg = "群组不存在!";
        }

        return respon;
    }

    // 创建群聊
    static respon CreatGroup(infor &infor, Database &Database)
    {
        int newGID;
        respon respon;
        struct respon notice;
        respon.status = FAILED;

        if (Database.CreateGroup(infor.id, infor.Targetgroupname, infor.Targetusers, newGID))
        {
            Database.AddToGroup(infor.id, newGID);
            respon.status = SUCCESS;
            respon.msg = "创建成功! 您的群聊ID: " + to_string(newGID);

            for (auto &mem : infor.Targetusers)
            {
                // 添加群聊邀请
                Database.AddToJoin(mem, newGID);
                if (Database.UserIsOnline(mem))
                {
                    notice.status = NOTICE;
                    notice.msg = infor.Targetgroupname + "群聊邀请: " + Database.GerUserName(infor.id);
                    string jsonrespon = JsonMsg::Json(notice);
                    IO::SendData(Database.GetUserSocket(mem), jsonrespon);
                }
                else // 用户不在线
                {
                    // 将消息加入待发送组
                    respon.status = SUCCESS;
                    Database.AddUnSendMsg(mem, infor.Targetgroupname + "群聊邀请: " + Database.GerUserName(infor.id));
                    // cout << "加入待发群组" << endl;
                }
            }
        }
        else
            respon.msg = "创建失败！";
        return respon;
    }

    // 退出群聊
    static respon QuitGroup(infor &infor, Database &Database)
    {
        respon respon;
        respon.status = FAILED;
        vector<int> admins = Database.GetGroupAdminsID(infor.Targetgroupid);

        if (Database.DelFromGroup(infor.id, infor.Targetgroupid))
        {
            // 是管理员
            if ((find(admins.begin(), admins.end(), infor.id) != admins.end()))
            {
                Database.DelGroupAdmin(infor.Targetgroupid, infor.id);
            }
            respon.status = SUCCESS;
            respon.msg = "退出成功！";
        }
        else
            respon.msg = "退出失败！";

        return respon;
    }

    // 客户端发送文件启动线程接收
    static respon SendFile(infor msg, Database &Database, Epoll &epoll)
    {
        respon respon;
        respon.status = FAILED;
        // 获取对方是否有该文件
        vector<string> Files = Database.GetFile(msg.Targetfriendid);
        for (const auto &file : Files)
        {
            if (file == msg.filename)
            {
                respon.msg = "该文件已发送！";
                return respon;
            }
        }

        thread recvthread(downfile, msg, &epoll);
        recvthread.detach();

        Database.AddFile(msg.Targetfriendid, msg.filename);
        respon.status = SUCCESS;
        return respon;
    }

    // 下载线程
    static void downfile(infor msg, Epoll *epoll)
    {
        auto logger = spdlog::get("server_logger");

        string filename = "../file/" + to_string(msg.Targetfriendid) + "-" + msg.filename;
        int fd = open(filename.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        if (fd < 0)
        {
            logger->error("文件无法打开！");
            return;
        }
        char buf[MAX_SIZE];
        int recvbytes = 0;
        while (1)
        {
            int ret = recv(msg.sockfd, buf, MAX_SIZE, 0);
            if (ret <= 0 && recvbytes == msg.bytes)
            {
                break;
            }
            else if (ret > 0)
            {
                recvbytes += ret;
                ssize_t total_bytes_written = 0;
                while (total_bytes_written < ret)
                {
                    ssize_t bytes_written = write(fd, buf + total_bytes_written, ret - total_bytes_written);
                    cout << bytes_written << endl;
                    if (bytes_written < 0)
                    {
                        logger->error("写入错误！");
                        return;
                    }
                    total_bytes_written += bytes_written;
                }
            }
        }
        epoll->AddEvent(msg.sockfd);
        close(fd);
    }

    // 查看可接收文件
    static respon Files(infor &msg, Database &Database)
    {
        respon respon;
        respon.status = FAILED;

        respon.Friends = Database.GetFile(msg.id);

        respon.status = SUCCESS;
        return respon;
    }

    // 接收文件 启动发送线程
    static respon RecvFile(infor &msg, Database &Database, Epoll &epoll)
    {
        respon respon;
        respon.status = FAILED;
        thread recvthread(filesend, msg, &epoll);
        recvthread.detach();

        string filename = "../file/" + to_string(msg.id) + "-" + msg.filename;
        struct stat st;
        if (stat(filename.c_str(), &st) != 0)
        {
            cout << "文件不存在！" << endl;
            return respon;
        }

        respon.bytes = st.st_size;
        Database.DelFile(msg.id, msg.filename);
        respon.status = SUCCESS;
        return respon;
    }

    // 发送线程
    static void filesend(infor msg, Epoll *epoll)
    {
        this_thread::sleep_for(chrono::seconds(1));
        auto logger = spdlog::get("server_logger");
        string filename = "../file/" + to_string(msg.id) + "-" + msg.filename;
        struct stat st;
        if (stat(filename.c_str(), &st) != 0)
        {
            cout << "文件不存在！" << endl;
            return;
        }

        int fd = open(filename.c_str(), O_RDONLY);
        if (fd < 0)
        {
            logger->error("文件无法打开！");
            return;
        }

        off_t fileSize = st.st_size;
        cout << "filesize " << fileSize << st.st_size << endl;
        off_t offset = 0;
        ssize_t sentBytes;
        while (offset < fileSize)
        {
            sentBytes = sendfile(msg.sockfd, fd, &offset, fileSize - offset);
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
        cout << "offset" << offset << endl;
        close(fd);
        remove(filename.c_str());

        epoll->AddEvent(msg.sockfd);
    }

    // 接收心跳
    static respon HeartBeat(infor &infor, Database &Database)
    {
        infor.id;
        respon res;
        Database.ChangHeart(infor.id, 1);

        res.status = HEART_BEAT;
        return res;
    }

    // 60s 归零一次 120s检测心跳
    static void CheckHeart(Database *Database)
    {
        auto logger = spdlog::get("server_logger");
        while (1)
        {
            this_thread::sleep_for(std::chrono::seconds(40));
            Database->ChangAllHeart(0);
            this_thread::sleep_for(std::chrono::seconds(60));
            queue<int> NoHeart = Database->GetNoHeart();
            while (!NoHeart.empty())
            {
                int no = NoHeart.front();
                NoHeart.pop();
                logger->warn("客户端{}心跳丢失", no);
                Database->ChangeStatus(no, OFFLINE);
                Database->DelNoHeart(no);
            }
        }
    }
};
