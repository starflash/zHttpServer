cmake_minimum_required(VERSION 3.5)
project(zHttpServer)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=gnu++11" COMPILER_SUPPORTS_GNUXX11)
CHECK_CXX_COMPILER_FLAG("-std=gnu++0X" COMPILER_SUPPORTS_GNUXX0X)
if(COMPILER_SUPPORTS_GNUXX11)
    set(COMPILER_STD_GNU  "-std=gnu++11")
elseif(COMPILER_SUPPORTS_GNUXX0X)
    set(COMPILER_STD_GNU  "-std=gnu++0x")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_STD_GNU} -Wno-deprecated -Wall -g -O0")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILER_STD_GNU} -Wno-deprecated -Wall -O2 -fno-strict-aliasing")
endif()

include_directories(.)

find_package(Threads REQUIRED)
if(Threads_FOUND)
    include_directories(${THREADS_INCLUDE_DIR})
endif(Threads_FOUND)

find_package(LibXml2 REQUIRED)
if(LIBXML2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
endif(LIBXML2_FOUND)

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIR})
endif(ZLIB_FOUND)

find_package(Lua51 REQUIRED)
if(Lua51_FOUND)
    include_directories(${LUA_INCLUDE_DIR})
endif(Lua51_FOUND)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(zhs)
add_subdirectory(HttpServer)