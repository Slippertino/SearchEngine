cmake_minimum_required (VERSION 3.8)

file(GLOB STEMMER_SOURCES "${CMAKE_CURRENT_LIST_DIR}/src_c/*.c"
						  "${CMAKE_CURRENT_LIST_DIR}/runtime/*.c"
						  "${CMAKE_CURRENT_LIST_DIR}/libstemmer/*.c")

add_library(libstemmer STATIC ${STEMMER_SOURCES})

target_include_directories(libstemmer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)