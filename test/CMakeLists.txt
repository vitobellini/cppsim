set(TEST_FILES ../test/Test.cpp)

add_executable (Test ${TEST_FILES})
target_link_libraries(Test cnpy)
target_link_libraries(Test Similarity)

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( Test ${ZLIB_LIBRARIES} )
endif()

find_package(Boost 1.67.0 COMPONENTS system filesystem thread unit_test_framework REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(Test ${Boost_LIBRARIES})
endif()

add_definitions (-DBOOST_TEST_DYN_LINK)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/input.npy DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/output.npy DESTINATION ${CMAKE_CURRENT_BINARY_DIR})