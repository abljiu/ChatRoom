#include <string>
#include <vector>
#include <queue>
using namespace std;
#define MAX_SIZE 10240
#define THREAD_POLL_NUM 20

const int HEARTBEAT_INTERVAL = 1000; // 1秒发送一次心跳包
const int HEARTBEAT_TIMEOUT = 3000;  // 3秒内没有收到心跳响应即认为连接断开
#define HEART_BEAT -5
// 登录前的任务类型
#define LOGIN -4
#define REGISTER -3
#define FINDPASSWORD -2
#define LOGOUT -1
#define QUIT 0

// 私聊
#define FRIEND_CHAT 1          // 选择好友进行聊天
#define ONLINE_FRIEND 2        // 查看在线好友
#define CHECK_FRIEND_APPLY 3   // 查看好友申请
#define CHECK_FRIEND_HISTORY 4 // 查看好友历史记录
#define DEL_FRIEND 5           // 删除好友
#define APPLY_FRIEND 6         // 申请添加好友
#define CHECK_FRIEND 7         // 查看好友
#define B_FRIEND 8             // 屏蔽好友
#define DEL_B_FRIEND 9         // 取消屏蔽好友
#define CHECK_BLIST 10       // 查看屏蔽好友
#define LOGEXIT 11             // 退出登录
// 群聊
#define CHECK_GROUP_APPLY 12   // 查看申请加群消息
#define CHECK_GROUP 13         // 查看加入群聊
#define GROUP_CHAT 14          // 群聊
#define CHECK_GROUP_HISTORY 15 // 查看群聊历史记录
#define CHECK_GROUPMEM 16      // 查看群聊成员
#define DEL_GROUP 17           // 解散群聊（群主）
#define DEL_ADMIN 18           // 删除群管理员（群主）
#define ADD_ADMIN 19           // 添加群管理员（群主）
#define DEL_MEM 20             // 删除普通成员（群主和管理）
#define CHECK_TO_JOIN 21       // 查看邀请群聊列表
#define JOIN_GROUP 22          // 申请加群
#define CREAT_GROUP 23         // 创建群聊
#define QUIT_GROUP 24          // 退出群聊
// 其它
#define SEND_FILE 25 // 发送文件
#define RECV_FILE 26 // 接收文件
#define FILES 27     // 查看文件

#define ADD_FRIEND 28    // 添加好友
#define ADD_MEM 29       // 同意加入群聊（群主和管理）
#define AGREE_TO_JOIN 30 // 同意群聊邀请
#define CHECK_FILE 31    // 查看文件列表
// 服务器发送是否成功
#define SUCCESS 50
#define FAILED 60
#define NOTICE 70
// 用户在线状态
#define OFFLINE 0 // 离线
#define ONLINE 1  // 在线s

struct infor
{
    int task = 0;          // 任务类型
    int status = 0;        // 状态
    int sockfd = 0;        // 自己sockfd
    int id = 0;            // 用户id
    string name = " ";     // 用户名称
    string password = " "; // 密码
    string propass = " ";  // 密保

    string filename = " ";        // 要保存的文件名
    string chatmessage = " ";     // 发送的消息
    string Targetgroupname = " "; // 目标对象名称
    string Targetfriendname = ""; // 目标对象id
    int Targetfriendid = 0;       // 目标对象id
    int Targetgroupid = 0;        // 目标群聊id
    int bytes = 0;                // 文件大小
    vector<int> Targetusers;      // 对象们
};

struct chatmsg
{
    string speaker = " ";
    string message = " ";
};
struct respon
{
    int status = 0;           // 是否成功或通知
    string msg = " ";         // 服务器响应消息'
    string name = " ";        // 用户名称
    int bytes = 0;            // 发送文件大小
    vector<string> Friends;   // 好友列表
    vector<int> FriendsID;    // 好友列表ID
    vector<string> Groups;    // 群组列表
    vector<int> GroupsID;     // 群组ID列表
    vector<chatmsg> chatmsgs; // 聊天记录
};
