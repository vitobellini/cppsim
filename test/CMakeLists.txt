set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_executable (Test test.cpp)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( Test ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )

find_package(Boost 1.67.0 COMPONENTS system filesystem thread unit_test_framework REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(Test ${Boost_LIBRARIES})
endif()


include_directories (${TEST_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIRS}
        )
add_definitions (-DBOOST_TEST_DYN_LINK)

target_link_libraries(Test Similarity)
target_link_libraries(Test cnpy)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/input.npy DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/output.npy DESTINATION ${CMAKE_CURRENT_BINARY_DIR})