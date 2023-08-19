#include "../src/head.hpp"
using namespace std;

mutex mtx;
condition_variable cv;
//  屏蔽  ctrl d
void ignore_eof()
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_cc[VEOF] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

int main(int argc, char *argv[])
{
    ignore_eof();

    // socket
    string ip = string(argv[1]);
    int port = atoi(argv[2]);
    Socket socket;
    socket.Connect(ip, port);
    Client client(ip, port, socket.sockfd);
    client.Run();
}


// 管理员查看加群消息 退群后取消管理员
// 查看加群选择不加入 管理员踢群主
// 文件不接收 文件接收错误
// 拉群复杂输入
// cmake 消息发多服务器阻塞 在线列表 删除缓存