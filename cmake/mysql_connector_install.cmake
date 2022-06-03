set(CONCPP_LIB_DIR ${SE_LIB_DIRS}/mysql-connector/lib64/vs14/debug)
set(CONCPP_INCLUDE_DIR ${SE_LIB_DIRS}/mysql-connector/include)

add_definitions(-DBUILD_STATIC=true -DCPPCONN_PUBLIC_FUNC=)

link_directories(${CONCPP_LIB_DIR})
include_directories(${CONCPP_INCLUDE_DIR})
