set(SOURCE_FILES ../main.cpp)

add_library (Similarity similarity.cpp similarity.h)
add_library (cnpy ../cnpy/cnpy.cpp)
add_executable(cppsim ${SOURCE_FILES})
target_link_libraries (cppsim cnpy Similarity)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( cppsim ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

find_package(Boost 1.67.0 COMPONENTS system filesystem thread program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(cppsim ${Boost_LIBRARIES})
endif()