CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT("thatlib" VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

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

################################################################
# STB
INCLUDE_DIRECTORIES(ext)

################################################################
# THATLIB
SOURCE_GROUP(math REGULAR_EXPRESSION src/math/.*)
SOURCE_GROUP(img REGULAR_EXPRESSION src/img/.*)

INCLUDE_DIRECTORIES ( src )

FILE(GLOB THAT_SOURCE 
src/math/*
src/img/*
)

SET(SOURCES src/img/stb_image.cpp src/img/ImRead.cpp src/img/ImWrite.cpp)

ADD_LIBRARY(thatlib STATIC ${SOURCES} ${THAT_SOURCE})