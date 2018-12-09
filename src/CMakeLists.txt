set(SOURCE_FILES ../main.cpp)

add_executable(cppsim ${SOURCE_FILES})
target_link_libraries (cppsim cnpy Similarity)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( cppsim ${ZLIB_LIBRARIES} )
endif()

find_package(Boost 1.67.0 COMPONENTS system filesystem thread program_options)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(cppsim ${Boost_LIBRARIES})
endif()