cmake_minimum_required (VERSION 3.8)

project (search_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SE_INSTALLERS ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(SE_LIB_DIRS ${CMAKE_CURRENT_LIST_DIR}/third_party)
set(SE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/search_engine)
set(SE_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake)

if (MSVC)
  add_compile_options(/bigobj)
else ()
  add_compile_options(-Wa,-mbig-obj)
endif ()

file(GLOB SE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/search_engine_analyzers)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thread_extensions)

include(${SE_CMAKE_DIR}/cpr_install.cmake)
include(${SE_CMAKE_DIR}/nlohmann_json_install.cmake)
include(${SE_CMAKE_DIR}/mysql_connector_install.cmake)

include_directories(${SE_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SE_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE cpr
                                              mysqlcppconn-static
                                              nlohmann_json
                                              search_engine_analyzers
                                              thread_extensions) 