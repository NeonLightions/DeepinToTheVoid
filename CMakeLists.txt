cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(DeepinToTheVoid)

# Include dependencies
include(packages/setup.cmake)

add_executable(Deepin 
    Main.cc
)
target_link_libraries(Deepin PRIVATE ${LIBS})
target_include_directories(Deepin PRIVATE 
    ${INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_features(Deepin PRIVATE cxx_std_20)

add_executable(Test 
    Test.cc
)
target_link_libraries(Test PRIVATE ${LIBS})
target_include_directories(Test PRIVATE 
    ${INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
target_compile_features(Test PRIVATE cxx_std_20)

