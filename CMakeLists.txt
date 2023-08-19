# 指定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(MyProject)

# 添加可执行文件: client
add_executable(

    client
    client/client.cc
    client/client.hpp
    client/friend.hpp
    client/group.hpp
    client/inside.hpp
    client/menu.hpp
)

# 添加可执行文件: server
add_executable(
    server

    server/server.cc
    server/server.hpp
    server/logic.hpp
)
target_link_libraries (
    server

    -lhiredis
    -lfmt
    -ljsoncpp
)
target_link_libraries(
    client

    -lhiredis
    -lfmt
    -ljsoncpp
)
# 添加库或源文件目录
# add_library(
#     common
#     src/database.hpp
#     src/head.hpp
#     src/json.hpp
#     src/queue.hpp
#     src/vector.hpp
#     src/epoll.hpp
#     src/IO.hpp
#     src/proto.hpp
#     src/sock.hpp
# )

# include_directories(/usr/include/hiredis)


# # 查找并链接 jsoncpp 库
# find_package(json-c REQUIRED)
# target_link_libraries(server PRIVATE json-c)
#
# # 查找并链接 hiredis 库
# # find_library( REQUIRED hiredis)
# find_package(hiredis REQUIRED)
# target_link_libraries(server  PRIVATE hiredis::hiredis)
#
#
# # 查找并链接 fmt 库
# find_package(fmt REQUIRED)
# target_link_libraries(server PRIVATE fmt::fmt)
#
# # 链接库到可执行文件
# target_link_libraries(client PRIVATE common)
# target_link_libraries(server PRIVATE common)
