set(SRC_LIST usercaller.cpp ../user.pb.cc)

add_executable(client ${SRC_LIST})
target_link_libraries(client protobuf myrpc)