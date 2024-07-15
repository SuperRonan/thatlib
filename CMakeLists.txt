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

set(THATLIB_FOLDERS 
	core 
	math
	img 
	utils 
	stl_ext
)

set(THATLIB_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(THATLIB_SOURCE_FOLDER "${THATLIB_ROOT_DIR}/src/")
set(THATLIB_INCLUDE_FOLDER "${THATLIB_ROOT_DIR}/include/")

set(THATLIB_INCLUDE_FOLDERS ${THATLIB_FOLDERS})
list(TRANSFORM THATLIB_INCLUDE_FOLDERS PREPEND "${THATLIB_INCLUDE_FOLDER}that/")

set(THATLIB_SOURCE_FOLDERS ${THATLIB_FOLDERS})
list(TRANSFORM THATLIB_SOURCE_FOLDERS PREPEND "${THATLIB_SOURCE_FOLDER}that/")

set(THATLIB_INCLUDE_FOLDERS_FILES ${THATLIB_INCLUDE_FOLDERS})
list(TRANSFORM THATLIB_INCLUDE_FOLDERS_FILES APPEND "/*")
set(THATLIB_SOURCE_FOLDERS_FILES ${THATLIB_SOURCE_FOLDERS})
list(TRANSFORM THATLIB_SOURCE_FOLDERS_FILES APPEND "/*")

file(GLOB THATLIB_SOURCES ${THATLIB_SOURCE_FOLDERS_FILES} ${THATLIB_INCLUDE_FOLDERS_FILES})

add_library(thatlib STATIC ${THATLIB_SOURCES})
set_target_properties(thatlib PROPERTIES LINKER_LANGUAGE CXX)


################################################################
# STB
target_include_directories(thatlib PUBLIC ext)

################################################################
# THATLIB
target_include_directories(thatlib PUBLIC ${THATLIB_INCLUDE_FOLDER})
target_compile_definitions(thatlib PUBLIC $<IF:$<CONFIG:Debug>,_ITERATOR_DEBUG_LEVEL=2,_ITERATOR_DEBUG_LEVEL=0>)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") 
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

option(THATLIB_SEPARATE_SOURCE_GROUP OFF)

foreach(folder IN LISTS THATLIB_FOLDERS)
	if(${THATLIB_SEPARATE_SOURCE_GROUP})
		source_group("sources/${folder}" REGULAR_EXPRESSION "${THATLIB_SOURCE_FOLDER}that/${folder}/.*")
		source_group("headers/${folder}" REGULAR_EXPRESSION "${THATLIB_INCLUDE_FOLDER}that/${folder}/.*")
	else()
		source_group(${folder} "(src|include)/that/${folder}/.*")
	endif()
endforeach()

install(TARGETS thatlib DESTINATION lib)