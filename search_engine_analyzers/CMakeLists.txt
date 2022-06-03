cmake_minimum_required (VERSION 3.8)

project (search_engine_analyzers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CONVERTERS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/converters/include)
set(HT_AUTOMATONS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/html_text_automatons/include)
set(TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/tools/include)
set(SEA_LIBS_DIR ${CMAKE_CURRENT_LIST_DIR}/libs)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs)

file(GLOB SEA_SOURCES "converters/*.cpp" 
					  "html_text_automatons/*.cpp"
					  "tools/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SEA_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${SEA_LIBS_DIR}/libstemmer/include
												  ${SEA_LIBS_DIR}/gumbo_parser/src
												  ${CONVERTERS_INCLUDE_DIR}
												  ${HT_AUTOMATONS_INCLUDE_DIR}
												  ${TOOLS_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE gumbo libstemmer)