cmake_minimum_required(VERSION 3.10)

project("thatlib" VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

###############################################################
#FIND_PACKAGE(ZLIB)
#IF(NOT ZLIB_FOUND)
#    SET(ZLIB_BUILD_STATIC_LIBS ON CACHE BOOL " " FORCE)
#    SET(ZLIB_BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
#    ADD_SUBDIRECTORY(ext/zlib)
#
#    SET(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/zlib")
#    SET(ZLIB_LIBRARY zlibstatic)
#    SET_PROPERTY(TARGET zlibstatic PROPERTY FOLDER "ext")
#    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/ext/zlib")
#    
#    INSTALL(TARGETS zlibstatic DESTINATION bin)
#ENDIF()
#INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

file(GLOB THAT_SOURCE 
	src/core/*
	src/math/*
	src/img/*
	src/utils/*
	src/stl_ext/*
)
add_library(thatlib STATIC ${THAT_SOURCE})
set_target_properties(thatlib PROPERTIES LINKER_LANGUAGE CXX)


################################################################
# STB
target_include_directories(thatlib PUBLIC ext)

################################################################
# THATLIB
target_compile_definitions(thatlib PUBLIC $<IF:$<CONFIG:Debug>,_ITERATOR_DEBUG_LEVEL=2,_ITERATOR_DEBUG_LEVEL=0>)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") 
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

source_group(core REGULAR_EXPRESSION src/core/.*)
source_group(math REGULAR_EXPRESSION src/math/.*)
source_group(img REGULAR_EXPRESSION src/img/.*)
source_group(utils REGULAR_EXPRESSION src/utils/.*)
source_group(stl_ext REGULAR_EXPRESSION src/stl_ext/.*)
target_include_directories(thatlib PUBLIC src)


install(TARGETS thatlib DESTINATION lib)