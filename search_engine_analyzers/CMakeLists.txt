cmake_minimum_required (VERSION 3.8)

project (search_engine_analyzers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(HT_AUTOMATONS_DIR ${CMAKE_CURRENT_LIST_DIR}/html_text_automatons)
set(TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/tools)
set(TEXT_PROPERTY_TYPES_DIR ${CMAKE_CURRENT_LIST_DIR}/text_property_types)
set(SEA_LIBS_DIR ${CMAKE_CURRENT_LIST_DIR}/libs)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE .)

target_link_libraries(${PROJECT_NAME} INTERFACE gumbo libstemmer)