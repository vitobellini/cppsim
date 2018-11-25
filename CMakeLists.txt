cmake_minimum_required(VERSION 3.8)
project(cppsim)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(cppsim ${SOURCE_FILES} cnpy.cpp)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( cppsim ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

find_package(Boost 1.68.0 COMPONENTS system filesystem thread program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(cppsim ${Boost_LIBRARIES})
endif()