cmake_minimum_required(VERSION 3.18.4)

set(CMAKE_BUILD_TYPE Debug)

# test for mm 
set(MMTEST mm-test)
set(SOURCES mm-test.cpp)
add_executable(${MMTEST} ${SOURCES})
target_link_libraries(${MMTEST} PUBLIC 
  gtest_main 
  mm 
)
add_test(NAME ${MMTEST} COMMAND "${MMTEST}")

# test for external link_libraries
set(EXTERNALTEST external-test)
set(SOURCES external-test.cpp)
add_executable(${EXTERNALTEST} ${SOURCES})
target_link_libraries(${EXTERNALTEST} PUBLIC 
  gtest_main 
  libexternal 
)
add_test(NAME ${EXTERNALTEST} COMMAND "${EXTERNALTEST}")
