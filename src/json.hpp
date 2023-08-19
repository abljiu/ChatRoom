#pragma once
#include "head.hpp"
using namespace std;

class JsonMsg
{
public:
    // 将发给服务器的数据序列化
    static string Json(const infor &msg)
    {
        Json::Value root;
        root["task"] = msg.task;
        root["status"] = msg.status;
        root["name"] = msg.name;
        root["id"] = msg.id;
        root["password"] = msg.password;
        root["propass"] = msg.propass;
        root["filename"] = msg.filename;
        root["chatmessage"] = msg.chatmessage;
        root["Targetgroupname"] = msg.Targetgroupname;
        root["Targetfriendname"] = msg.Targetfriendname;
        root["Targetfriendid"] = msg.Targetfriendid;
        root["Targetgroupid"] = msg.Targetgroupid;
        root["bytes"] = msg.bytes;

        for (const auto &Targetusers : msg.Targetusers)
        {
            root["readygroup"].append(Targetusers);
        }
        Json::FastWriter writer;
        string write = writer.write(root);
        return write;
    }

    // 服务器对接收到的数据反序列化
    static void UnJson(const string &jsonstring, infor &msg)
    {
        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonstring, root);
        msg.task = root["task"].asInt();
        msg.status = root["status"].asInt();
        msg.id = root["id"].asInt();
        msg.name = root["name"].asString();
        msg.password = root["password"].asString();
        msg.propass = root["propass"].asString();
        msg.filename = root["filename"].asString();
        msg.chatmessage = root["chatmessage"].asString();
        msg.Targetgroupname = root["Targetgroupname"].asString();
        msg.Targetfriendname = root["Targetfriendname"].asString();
        msg.Targetfriendid = root["Targetfriendid"].asInt();
        msg.Targetgroupid = root["Targetgroupid"].asInt();
        int size = root["buffer"].toStyledString().size();
        msg.bytes = root["bytes"].asInt();
        for (const auto &value : root["readygroup"])
        {
            msg.Targetusers.push_back(value.asInt());
        }
    }
    // 将服务器的响应序列化
    static string Json(const respon &respon)
    {
        Json::Value root;
        root["status"] = respon.status;
        root["msg"] = respon.msg;
        root["name"] = respon.name;
        root["bytes"] = respon.bytes;
        for (const auto &Friends : respon.Friends)
        {
            root["Friends"].append(Friends);
        }
        for (const auto &FriendsID : respon.FriendsID)
        {
            root["FriendsID"].append(FriendsID);
        }
        for (const auto &Groups : respon.Groups)
        {
            root["Groups"].append(Groups);
        }
        for (const auto &GroupsID : respon.GroupsID)
        {
            root["GroupsID"].append(GroupsID);
        }
        // 先将chatmsg序列化成jsonstring,在存入root[jsonchatmsg]
        for (const auto &chatmsg : respon.chatmsgs)
        {
            string jsonchatmsg = Json(chatmsg);
            root["jsonchatmsg"].append(jsonchatmsg);
        }

        Json::FastWriter writer;
        string write = writer.write(root);
        return write;
    }

    // 客户端对收到的服务器响应反序列化
    static void UnJson(const string &jsonstring, respon &respon)
    {
        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonstring, root);
        respon.status = root["status"].asInt();
        respon.msg = root["msg"].asString();
        respon.name = root["name"].asString();
        respon.bytes = root["bytes"].asInt();
        for (const auto &value : root["Friends"])
        {
            respon.Friends.push_back(value.asString());
        }
        for (const auto &value : root["FriendsID"])
        {
            respon.FriendsID.push_back(value.asInt());
        }
        for (const auto &value : root["Groups"])
        {
            respon.Groups.push_back(value.asString());
        }
        for (const auto &value : root["GroupsID"])
        {
            respon.GroupsID.push_back(value.asInt());
        }
        // 先将jsonchatmsg转化为chatmsg再存入respon
        for (const auto &jsonchatmsg : root["jsonchatmsg"])
        {
            struct chatmsg chatmsg;
            UnJson(jsonchatmsg.asString(), chatmsg);
            respon.chatmsgs.push_back(chatmsg);
        }
    }

    // 对chatmsg序列化存入respon
    static string Json(const chatmsg &chatmsg)
    {
        Json::Value root;
        root["speaker"] = chatmsg.speaker;
        root["message"] = chatmsg.message;

        Json::FastWriter writer;
        string write = writer.write(root);
        return write;
    }

    // 将jsonchatmsg反序列化
    static void UnJson(const string &jsonstring, chatmsg &chatmsg)
    {
        Json::Reader reader;
        Json::Value root;
        reader.parse(jsonstring, root);
        chatmsg.speaker = root["speaker"].asString();
        chatmsg.message = root["message"].asString();
    }
};
