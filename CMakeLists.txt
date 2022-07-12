cmake_minimum_required (VERSION 3.8)

project (search_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SE_LIB_DIRS ${CMAKE_CURRENT_LIST_DIR}/libs)
set(SE_CORE ${CMAKE_CURRENT_LIST_DIR}/core)
set(SE_EXTENSIONS ${CMAKE_CURRENT_LIST_DIR}/extensions)
set(SE_ENGINE ${CMAKE_CURRENT_LIST_DIR}/engine)
set(SE_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)

set(SE_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake)

if (MSVC)
  add_compile_options(/bigobj)
else ()
  add_compile_options(-Wa,-mbig-obj)
endif ()

add_subdirectory(${SE_LIB_DIRS}/gumbo_parser)

add_subdirectory(${SE_LIB_DIRS}/libstemmer)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${SE_LIB_DIRS}/nlohmann_json)

include(${SE_CMAKE_DIR}/cpr_install.cmake)
include(${SE_CMAKE_DIR}/mysql_connector_install.cmake)

file(GLOB_RECURSE SE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SE_SOURCES})


#include_directories(${SE_CORE} ${SE_EXTENSIONS} ${SE_ENGINE} ${SE_INCLUDE} )
target_include_directories(${PROJECT_NAME} PRIVATE ${SE_CORE} ${SE_EXTENSIONS} ${SE_ENGINE} ${SE_INCLUDE} )

target_link_libraries(${PROJECT_NAME} PRIVATE cpr
                                              mysqlcppconn-static
                                              nlohmann_json
                                              libstemmer
                                              gumbo)

