aux_source_directory(. SRC_LIST)

add_library(myrpc  ${SRC_LIST})
target_link_libraries(myrpc muduo_net muduo_base)
# 给myrpc增加 -fPIC选项
# add_compile_options(-fPIC)

