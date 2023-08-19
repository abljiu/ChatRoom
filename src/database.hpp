#pragma once
#include "head.hpp"

class Database
{
private:
    redisContext *redis;
    vector<string> OnlineUsers;
    vector<int> OnlineUsersID;

    // 聊天列表的key为 chatlist
    int newchatnum;

public:
    Database()
    {
        redis = redisConnect("127.0.0.1", 6379);
        if (redis == NULL || redis->err)
        {
            perror("Redis 连接错误！");
        }
        redisReply *reply = (redisReply *)redisCommand(redis, "HSET chatlist recver-sender 0");
        if (reply->type == REDIS_REPLY_ERROR)
        {
            perror("Redis 聊天记录列表初始化错误！");
        }
        freeReplyObject(reply);

        if (!this->OffLine())
            perror("初始化错误");
    }
    ~Database()
    {

        // redisReply *reply = (redisReply *)redisCommand(redis, " FLUSHALL");

        if (redis != NULL)
        {
            redisFree(redis);
        }
    }
    // 设置为全下线
    bool OffLine()
    {
        vector<int> Online;

        Online = this->GetOlineUserID();

        for (const auto &user : Online)
        {
            if (!this->ChangeStatus(user, 0))
                return false;
        }
        return true;
    }

    //  存储用户和密码和密保 设置默认在线状态
    bool AddUser(const string username, const string password, const string propass, int &newUID)
    {
        // 获取最新id
        redisReply *reply = (redisReply *)redisCommand(redis, "GET newUID");
        if (reply->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply);
            return false;
        }
        else
        {
            newUID = atoi(reply->str);
        }
        reply = (redisReply *)redisCommand(redis, "HMSET %d username %s password %s propass %s isonline %d", newUID, username.c_str(), password.c_str(), propass.c_str(), OFFLINE);
        // 存储失败
        if (reply->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply);
            return false;
        }
        reply = (redisReply *)redisCommand(redis, "SET newUID %d", newUID + 1);
        if (reply->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 删除用户
    bool DelUser(const int userid)
    {

        redisReply *reply = (redisReply *)redisCommand(redis, "DEL %d", userid);
        if (reply->integer > 0)
        {
            freeReplyObject(reply);
            return true;
        }
        else
        {
            freeReplyObject(reply);
            return false;
        }
    }

    // 查询用户
    bool FindUser(const int userid)
    {

        redisReply *reply = (redisReply *)redisCommand(redis, "EXISTS %d", userid);
        if (reply == NULL)
        {
            std::cerr << "EXISTS 命令执行失败" << std::endl;
            redisFree(redis);
            return false;
        }
        if (reply->integer == 1)
        {
            return true;
        }
        return false;
    }

    // 根据id获取用户名
    string GerUserName(const int userid)
    {
        string name;
        redisReply *reply = (redisReply *)redisCommand(redis, "HGET %d username", userid);
        if (reply == NULL)
        {
            return name;
        }
        else
        {
            name = string(reply->str);
            return name;
        }
    }

    // 查询用户在线状态
    bool UserIsOnline(const int userid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "HGET %d isonline ", userid);
        if (reply == NULL)
        {
            return false;
        }
        if (atoi(reply->str) == ONLINE)
        {
            freeReplyObject(reply);
            return true;
        }
        freeReplyObject(reply);
        return false;
    }

    // 获取用户密码
    string GetPassword(const int userid)
    {
        string password = "NOT FIND";
        redisReply *reply = (redisReply *)redisCommand(redis, "HGET %d password", userid);
        if (reply->type == REDIS_REPLY_STRING)
        {
            password = reply->str;
        }
        freeReplyObject(reply);
        return password;
    }

    // 获取用户密保
    string GetPropass(const int userid)
    {
        string propass = "NOT FIND";
        redisReply *reply = (redisReply *)redisCommand(redis, "HGET %d propass", userid);
        if (reply->type == REDIS_REPLY_STRING)
        {
            propass = reply->str;
        }
        freeReplyObject(reply);
        return propass;
    }

    // 获取在线用户
    vector<string> GetOlineUser()
    {
        redisReply *keysReply = (redisReply *)redisCommand(redis, "KEYS *");
        if (keysReply == NULL)
        {
            std::cerr << "KEYS 命令执行失败" << std::endl;
            return OnlineUsers;
        }
        // 遍历所有键
        if (keysReply->type == REDIS_REPLY_ARRAY)
        {
            for (size_t i = 0; i < keysReply->elements; ++i)
            {
                char *key = keysReply->element[i]->str;

                // 使用 HGET 命令获取指定字段的值
                redisReply *hgetReply = (redisReply *)redisCommand(redis, "HGET %s isonline", key);
                if (hgetReply != NULL && hgetReply->type == REDIS_REPLY_STRING)
                {
                    if (!strcmp(hgetReply->str, "1"))
                    {
                        OnlineUsers.push_back(this->GerUserName(atoi(key)));
                    }
                }
                freeReplyObject(hgetReply);
            }
        }
        freeReplyObject(keysReply);
        return OnlineUsers;
    }

    // 获取在线用户ID
    vector<int> GetOlineUserID()
    {
        redisReply *keysReply = (redisReply *)redisCommand(redis, "KEYS *");
        if (keysReply == NULL)
        {
            std::cerr << "KEYS 命令执行失败" << std::endl;
            return OnlineUsersID;
        }
        // 遍历所有键
        if (keysReply->type == REDIS_REPLY_ARRAY)
        {
            for (size_t i = 0; i < keysReply->elements; ++i)
            {
                char *key = keysReply->element[i]->str;

                // 使用 HGET 命令获取指定字段的值
                redisReply *hgetReply = (redisReply *)redisCommand(redis, "HGET %s isonline", key);
                if (hgetReply != NULL && hgetReply->type == REDIS_REPLY_STRING)
                {
                    if (!strcmp(hgetReply->str, "1"))
                    {
                        OnlineUsersID.push_back(atoi(key));
                    }
                }
                freeReplyObject(hgetReply);
            }
        }
        freeReplyObject(keysReply);
        return OnlineUsersID;
    }

    // 修改用户在线状态
    bool ChangeStatus(const int userid, int newstatus)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "HSET %d isonline %d", userid, newstatus);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 储存每个用户连接时的socket
    bool SetUserSocket(const int userid, int sockfd)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SET %d:socket  %d", userid, sockfd);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 退出时删除socket
    bool DelUserSocket(const int userid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "Del %d:socket ", userid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 根据socket获取id
    int GetIDBySocket(int sockfd)
    {
        int id = 0;
        redisReply *keysReply = (redisReply *)redisCommand(redis, "KEYS *");
        if (keysReply == NULL)
        {
            std::cerr << "KEYS 命令执行失败" << std::endl;
            return id;
        }
        // 遍历所有键
        if (keysReply->type == REDIS_REPLY_ARRAY)
        {
            for (size_t i = 0; i < keysReply->elements; ++i)
            {
                char *key = keysReply->element[i]->str;

                // 使用 HGET 命令获取指定字段的值
                redisReply *hgetReply = (redisReply *)redisCommand(redis, "GET %s ", key);
                if (hgetReply != NULL && hgetReply->type == REDIS_REPLY_STRING)
                {
                    if (atoi(hgetReply->str) == sockfd)
                    {
                        id = atoi(key);
                        return id;
                    }
                }
                freeReplyObject(hgetReply);
            }
        }
        freeReplyObject(keysReply);
        return id;
    }

    // 获取每个用户的sockfd
    int GetUserSocket(const int userid)
    {
        int sockfd = -1;
        redisReply *reply = (redisReply *)redisCommand(redis, "GET %d:socket ", userid);
        if (reply->type == REDIS_REPLY_STRING)
        {
            sockfd = atoi(reply->str);
            freeReplyObject(reply);
        }
        return sockfd;
    }

    // 添加好友
    bool AddFriend(const int userid, const int friendid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SADD %d:friends %d", userid, friendid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 删除好友
    bool DelFriend(const int userid, const int friendid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SREM %d:friends %d", userid, friendid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        reply = (redisReply *)redisCommand(redis, "SREM %d:friends %d", friendid, userid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取好友列表
    vector<string> GetFriends(const int userid)
    {
        vector<string> friends;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:friends", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            friends.push_back(this->GerUserName(atoi(reply->element[i]->str)));
        }
        freeReplyObject(reply);
        return friends;
    }

    // 获取好友列表ID
    vector<int> GetFriendsID(const int userid)
    {
        vector<int> friends;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:friends", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            friends.push_back(atoi(reply->element[i]->str));
        }
        freeReplyObject(reply);
        return friends;
    }

    // 添加屏蔽好友
    bool AddBFriend(const int userid, const int friendid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SADD %d:Bfriends %d", userid, friendid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 删除屏蔽好友
    bool DelBFriend(const int userid, const int friendid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SREM %d:Bfriends %d", userid, friendid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取好友屏蔽列表
    vector<string> GetBFriends(const int userid)
    {
        vector<string> Bfriends;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:Bfriends", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            Bfriends.push_back(this->GerUserName(atoi(reply->element[i]->str)));
        }
        freeReplyObject(reply);
        return Bfriends;
    }

    // 获取好友屏蔽列表ID
    vector<int> GetBFriendsID(const int userid)
    {
        vector<int> Bfriends;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:Bfriends", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            Bfriends.push_back(atoi(reply->element[i]->str));
        }
        freeReplyObject(reply);
        return Bfriends;
    }
    // 申请添加好友
    bool ApplyAddFriend(const int userid, const int friendid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SADD %d:friendApply %d", userid, friendid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 删除申请添加好友
    bool DelAddFriend(const int userid, const int friendid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SREM %d:friendApply %d", userid, friendid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取好友申请列表
    vector<string> GetFriendsApply(const int userid)
    {
        vector<string> friendApply;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:friendApply", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            cout << "找到了！" << endl;
            friendApply.push_back(this->GerUserName(atoi(reply->element[i]->str)));
        }
        freeReplyObject(reply);
        return friendApply;
    }
    // 获取好友申请列表ID
    vector<int> GetFriendsApplyID(const int userid)
    {
        vector<int> friendApply;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:friendApply", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            cout << "找到了！" << endl;
            friendApply.push_back(atoi(reply->element[i]->str));
        }
        freeReplyObject(reply);
        return friendApply;
    }

    // 查找两人间是否存在聊天记录返回chatnum
    bool CheckRecord(const int user1id, const int user2id, int &chatnum)
    {
        hash<string> hasher;
        string chat;

        // 大加小组合对应唯一两人chatnum
        if (user1id > user2id)
        {
            chat = to_string(user1id) + "&" + to_string(user2id);
        }
        else
        {
            chat = to_string(user2id) + "&" + to_string(user1id);
        }

        redisReply *reply = (redisReply *)redisCommand(redis, "HGET chatlist %s ", chat.c_str());

        if (reply->type == REDIS_REPLY_STRING)
        {
            chatnum = atoi(reply->str);
            return true;
        }
        else
            return false;
    }

    // 添加聊天记录 用jsonstring储存
    bool AddChatRecord(const int user1id, const int user2id, const string &chatRecord)
    {
        hash<string> hasher;
        redisReply *reply;
        string chat;
        // 如果两人首次聊天
        if (!CheckRecord(user1id, user2id, newchatnum))
        {
            // 大加小组合对应唯一两人chatnum
            if (user1id > user2id)
            {
                newchatnum = hasher((to_string(user1id) + "&" + to_string(user2id)).c_str());
                chat = to_string(user1id) + "&" + to_string(user2id);
                reply = (redisReply *)redisCommand(redis, "HSET chatlist %s %d", chat.c_str(), newchatnum);
            }
            else
            {
                newchatnum = hasher((to_string(user2id) + "&" + to_string(user1id)).c_str());
                chat = to_string(user2id) + "&" + to_string(user1id);
                reply = (redisReply *)redisCommand(redis, "HSET chatlist %s %d", chat.c_str(), newchatnum);
            }
        }

        reply = (redisReply *)redisCommand(redis, "LPUSH %d %s", newchatnum, chatRecord.c_str());
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取聊天记录
    vector<chatmsg> GetChatRecords(const int user1id, const int user2id)
    {
        vector<chatmsg> chatRecords;
        if (CheckRecord(user1id, user2id, newchatnum))
        {
            redisReply *reply = (redisReply *)redisCommand(redis, "LRANGE %d 0 -1", newchatnum);

            for (size_t i = 0; i < reply->elements; i++)
            {
                chatmsg chatmsg;
                JsonMsg::UnJson(reply->element[i]->str, chatmsg);
                chatRecords.push_back(chatmsg);
            }
            freeReplyObject(reply);
        }
        return chatRecords;
    }

    // 获取newGID
    int GetNewUID()
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "GET newUID");
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return -1;
        }
        else
        {
            freeReplyObject(reply);
            return atoi(reply->str);
        }
    }

    // 获取newGID
    int GetNewGID()
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "GET newGID");
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return -1;
        }
        else
        {
            freeReplyObject(reply);
            return atoi(reply->str);
        }
    }

    // 创建群聊
    bool CreateGroup(const int userid, const string groupname, vector<int> mems, int &newGID)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "GET newGID");
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        else
        {
            newGID = atoi(reply->str);
        }
        reply = (redisReply *)redisCommand(redis, "HMSET group:%d GroupName %s GroupMaster %d", newGID, groupname.c_str(), userid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        reply = (redisReply *)redisCommand(redis, "SET newGID %d", newGID + 1);
        freeReplyObject(reply);
        return true;
    }

    // 根据id获取群名
    string GerGroupName(const int groupid)
    {
        string name;
        redisReply *reply = (redisReply *)redisCommand(redis, "HGET group:%d GroupName", groupid);
        if (reply == NULL)
        {
            return name;
        }
        else
        {
            name = string(reply->str);
            return name;
        }
    }

    // 申请添加群聊
    bool ApplyAddGroup(const int groupid, const int userid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SADD %d:GroupApply %d", groupid, userid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 删除申请添加群聊
    bool DelAddGroup(const int groupid, const int userid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SREM %d:GroupApply %d", groupid, userid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取群聊申请列表
    vector<string> GetGroupsApply(const int groupid)
    {
        vector<string> GroupApply;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:GroupApply", groupid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            GroupApply.push_back(this->GerUserName(atoi(reply->element[i]->str)));
        }
        freeReplyObject(reply);
        return GroupApply;
    }

    // 获取群聊申请列表ID
    vector<int> GetGroupsApplyID(const int groupid)
    {
        vector<int> GroupApply;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:GroupApply", groupid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            GroupApply.push_back(atoi(reply->element[i]->str));
        }
        freeReplyObject(reply);
        return GroupApply;
    }

    // 添加群聊邀请
    bool AddToJoin(const int userid, const int groupid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SADD %d:grouptojoin %d", userid, groupid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 删除群聊邀请
    bool DelToJoin(const int userid, const int groupid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SREM %d:grouptojoin %d", userid, groupid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 同意添加群聊
    bool AddToGroup(const int userid, const int groupid)
    {

        redisReply *reply1 = (redisReply *)redisCommand(redis, "SADD %d:groupmem %d", groupid, userid);
        redisReply *reply2 = (redisReply *)redisCommand(redis, "SADD %d:groups %d", userid, groupid);
        if (reply1 == NULL || reply2 == NULL)
        {
            freeReplyObject(reply1);
            freeReplyObject(reply2);
            return false;
        }
        freeReplyObject(reply1);
        freeReplyObject(reply2);
        return true;
    }

    // 群聊存在
    bool ExistsGroup(const int groupid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "EXISTS group:%d ", groupid);
        if (reply->integer == 1)
        {
            freeReplyObject(reply);
            return true;
        }
        freeReplyObject(reply);
        return false;
    }
    // 查看群聊邀请
    vector<string> CheckToJoin(const int userid)
    {
        vector<string> tojoingroups;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:grouptojoin ", userid);
        if (reply != NULL)
        {
            for (size_t i = 0; i < reply->elements; i++)
            {
                tojoingroups.push_back(this->GerGroupName(atoi(reply->element[i]->str)));
            }
        }
        freeReplyObject(reply);
        return tojoingroups;
    }

    // 查看群聊邀请ID
    vector<int> CheckToJoinID(const int userid)
    {
        vector<int> tojoingroups;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:grouptojoin ", userid);
        if (reply != NULL)
        {
            for (size_t i = 0; i < reply->elements; i++)
            {
                tojoingroups.push_back(atoi(reply->element[i]->str));
            }
        }
        freeReplyObject(reply);
        return tojoingroups;
    }

    // 踢出群聊
    bool DelFromGroup(const int userid, const int groupid)
    {

        redisReply *reply1 = (redisReply *)redisCommand(redis, "SREM %d:groupmem %d", groupid, userid);
        redisReply *reply2 = (redisReply *)redisCommand(redis, "SREM %d:groups %d", userid, groupid);
        if (reply1 == NULL || reply2 == NULL)
        {
            freeReplyObject(reply1);
            freeReplyObject(reply2);
            return false;
        }
        freeReplyObject(reply1);
        freeReplyObject(reply2);
        return true;
    }

    // 添加群管理员
    bool AddAdmins(const int groupid, vector<int> admins)
    {
        redisReply *reply;
        for (const auto &admin : admins)
        {
            reply = (redisReply *)redisCommand(redis, "SADD %d:GroupAdmin  %d", groupid, admin);
            if (reply == NULL)
            {
                freeReplyObject(reply);
                return false;
            }
        }
        freeReplyObject(reply);
        return true;
    }

    // 解散群聊
    bool DelGroup(const int groupid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "DEL group:%d", groupid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        reply = (redisReply *)redisCommand(redis, "DEL %d:groupmem", groupid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        reply = (redisReply *)redisCommand(redis, "DEL %d:GroupRecord", groupid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        reply = (redisReply *)redisCommand(redis, "DEL %d:GroupAdmin", groupid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        reply = (redisReply *)redisCommand(redis, "DEL %d:GroupRecord", groupid);
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取群聊成员
    vector<string> GetGroupMem(const int groupid)
    {
        vector<string> groupmem;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:groupmem", groupid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            groupmem.push_back(this->GerUserName(atoi(reply->element[i]->str)));
        }
        freeReplyObject(reply);
        return groupmem;
    }

    // 获取群聊成员ID
    vector<int> GetGroupMemID(const int groupid)
    {
        vector<int> groupmem;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:groupmem", groupid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            groupmem.push_back(atoi(reply->element[i]->str));
        }
        freeReplyObject(reply);
        return groupmem;
    }
    // 获取群主
    string GetGroupMaster(const int groupid)
    {
        string master = " ";
        redisReply *reply = (redisReply *)redisCommand(redis, "HGET group:%d GroupMaster", groupid);
        if (reply->type == REDIS_REPLY_STRING)
        {
            master = this->GerUserName(atoi(reply->str));
        }
        freeReplyObject(reply);
        return master;
    }

    // 获取群主ID
    int GetGroupMasterID(const int groupid)
    {
        int master = 0;
        redisReply *reply = (redisReply *)redisCommand(redis, "HGET group:%d GroupMaster", groupid);
        if (reply->type == REDIS_REPLY_STRING)
        {
            master = atoi(reply->str);
        }
        freeReplyObject(reply);
        return master;
    }

    // 获取群聊管理员
    vector<string> GetGroupAdmins(const int groupid)
    {
        vector<string> groupadmins;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:GroupAdmin", groupid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            groupadmins.push_back(this->GerUserName(atoi(reply->element[i]->str)));
        }
        freeReplyObject(reply);
        return groupadmins;
    }

    // 获取群聊管理员ID
    vector<int> GetGroupAdminsID(const int groupid)
    {
        vector<int> groupadmins;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:GroupAdmin", groupid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            groupadmins.push_back(atoi(reply->element[i]->str));
        }
        freeReplyObject(reply);
        return groupadmins;
    }

    // 删除群管理员
    bool DelGroupAdmin(const int groupid, const int adminid)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SREM %d:GroupAdmin ", groupid, adminid);
        if (reply == NULL)
        {
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取已加入群聊
    vector<string> GetGroupIn(const int userid)
    {
        vector<string> groups;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:groups", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            groups.push_back(this->GerGroupName(atoi(reply->element[i]->str)));
        }
        freeReplyObject(reply);
        return groups;
    }

    // 获取已加入群聊 ID
    vector<int> GetGroupInID(const int userid)
    {
        vector<int> groups;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:groups", userid);
        for (size_t i = 0; i < reply->elements; i++)
        {
            groups.push_back(atoi(reply->element[i]->str));
        }
        freeReplyObject(reply);
        return groups;
    }

    // 添加群聊记录 用jsonstring储存
    bool AddGroupRecord(const int groupid, const string &chatRecord)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "LPUSH %d:GroupRecord %s", groupid, chatRecord.c_str());
        if (reply == NULL)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 查看群聊历史记录
    vector<chatmsg> GetGroupRecords(const int groupid)
    {
        vector<chatmsg> GrpupRecords;
        redisReply *reply = (redisReply *)redisCommand(redis, "LRANGE %d:GroupRecord 0 -1", groupid);

        for (size_t i = 0; i < reply->elements; i++)
        {
            chatmsg chatmsg;
            JsonMsg::UnJson(reply->element[i]->str, chatmsg);
            GrpupRecords.push_back(chatmsg);
        }
        freeReplyObject(reply);
        return GrpupRecords;
    }

    //  存储待发送消息
    bool AddUnSendMsg(const int sendid, const string sendmsg)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "LPUSH %d:Unsendmsg %s ",
                                                       sendid, sendmsg.c_str());
        // 存储失败
        if (reply->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取用户待发送消息
    bool GetUnSendMsg(const int sendid, queue<string> &sendmsg)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "LPOP %d:Unsendmsg  ",
                                                       sendid);
        if (reply->type != REDIS_REPLY_NIL)
        {
            sendmsg.push(reply->str);
            while (reply->type != REDIS_REPLY_NIL)
            {
                reply = (redisReply *)redisCommand(redis, "LPOP %d:Unsendmsg  ", sendid);
                if (reply->type != REDIS_REPLY_NIL)
                    sendmsg.push(reply->str);
            }
            freeReplyObject(reply);
            return true;
        }
        else
        {
            freeReplyObject(reply);
            return false;
        }
    }

    //  存储待接收文件
    bool AddFile(const int userid, const string file)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SADD %d:Files %s ",
                                                       userid, file.c_str());
        // 存储失败
        if (reply->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }



    //  删除待接收文件
    bool DelFile(const int userid, const string file)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "SREM %d:Files %s",
                                                       userid, file.c_str());
        // 删除失败
        if (reply->type == REDIS_REPLY_ERROR)
        {
            freeReplyObject(reply);
            return false;
        }
        freeReplyObject(reply);
        return true;
    }

    // 获取待接收文件
    vector<string> GetFile(const int userid)
    {
        vector<string> Files;
        redisReply *reply = (redisReply *)redisCommand(redis, "SMEMBERS %d:Files ", userid);

        for (size_t i = 0; i < reply->elements; i++)
        {
            Files.push_back(reply->element[i]->str);
        }
        freeReplyObject(reply);
        return Files;
    }

    // 更改心跳计时
    bool ChangHeart(const int userid, const int heart)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "HSET HeartBeat %d %d ", userid, heart);
        if (reply != NULL)
        {
            freeReplyObject(reply);
            return true;
        }
        else
        {
            freeReplyObject(reply);
            return false;
        }
    }

    // 更改所有心跳计时
    bool ChangAllHeart(const int heart)
    {
        redisReply *keysReply = (redisReply *)redisCommand(redis, "HKEYS HeartBeat ");
        if (keysReply->type == REDIS_REPLY_ARRAY)
        {
            for (size_t i = 0; i < keysReply->elements; ++i)
            {
                char *key = keysReply->element[i]->str;

                // 使用 HSET 命令设置指定字段的值
                redisReply *Reply = (redisReply *)redisCommand(redis, "HSET HeartBeat %s %d", key, heart);
                freeReplyObject(Reply);
            }
            freeReplyObject(keysReply);
            return true;
        }
        else
        {
            freeReplyObject(keysReply);
            return false;
        }
    }

    // 返回为0的心跳计时
    queue<int> GetNoHeart()
    {
        queue<int> Noheart;
        redisReply *keysReply = (redisReply *)redisCommand(redis, "HKEYS HeartBeat ");
        if (keysReply->type == REDIS_REPLY_ARRAY)
        {
            for (size_t i = 0; i < keysReply->elements; ++i)
            {
                char *key = keysReply->element[i]->str;

                // 使用 HGET 命令获取指定字段的值
                redisReply *hgetReply = (redisReply *)redisCommand(redis, "HGET HeartBeat %s", key);
                if (atoi(hgetReply->str) == 0)
                {
                    Noheart.push(atoi(key));
                }
                freeReplyObject(hgetReply);
            }
        }
        freeReplyObject(keysReply);
        return Noheart;
    }

    // 删除心跳列表中某个用户
    bool DelNoHeart(const int userid)
    {
        redisReply *Reply = (redisReply *)redisCommand(redis, "HDEL HeartBeat %d ", userid);
        if (Reply->type == REDIS_REPLY_INTEGER)
        {
            if (Reply->integer > 0)
            {
                freeReplyObject(Reply);
                return true;
            }
            else
            {
                freeReplyObject(Reply);
                return false;
            }
        }
        freeReplyObject(Reply);
        return false;
    }
};