set (CPR_BUILD_TESTS_SSL OFF)
set (BUILD_CPR_TESTS OFF)
set (CMAKE_USE_OPENSSL OFF)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG db351ffbbadc6c4e9239daaa26e9aefa9f0ec82d) 
FetchContent_MakeAvailable(cpr)