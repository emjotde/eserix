cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(eserix CXX)
SET(CMAKE_CXX_FLAGS " -std=c++11 -g -O3 -funroll-loops -Wno-unused-result -Wno-deprecated")

include_directories(${eserix_SOURCE_DIR})
set(EXT_LIBS)

find_package(Boost COMPONENTS system filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(EXT_LIBS ${EXT_LIBS} ${Boost_LIBRARIES})
else(Boost_FOUND)
    message(SEND_ERROR "Cannot find Boost libraries. Terminating." )
endif(Boost_FOUND)

find_package(PCRECPP)
if(PCRECPP_FOUND)
    include_directories(${PCRECPP_INCLUDE_DIR})
    set(EXT_LIBS ${EXT_LIBS} ${PCRECPP_LIBRARIES})
else(PCRECPP_FOUND)
    message(SEND_ERROR "Cannot find PCRECPP library. Terminating." )
endif(PCRECPP_FOUND)

include_directories(${eserix_SOURCE_DIR}/src)
add_subdirectory(src)