cmake_minimum_required(VERSION 3.14)
project(LoadBalancerSimulation CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set include directories
include_directories(include)

# Gather source files for the LoadBalancer library
set(LB_SOURCES
    src/Server.cpp
    src/LoadBalancer.cpp
    src/RoundRobinStrategy.cpp
    src/LeastConnectionsStrategy.cpp
    src/RandomStrategy.cpp
    src/WeightedRoundRobinStrategy.cpp
)

# Create the library
add_library(LBLibrary ${LB_SOURCES})
target_compile_features(LBLibrary PUBLIC cxx_std_17)
# pthread is required for threading in C++
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(LBLibrary PUBLIC Threads::Threads)

# Add the main simulation executable
add_executable(LoadBalancerSimulation src/main.cpp)
target_link_libraries(LoadBalancerSimulation PRIVATE LBLibrary)

# Setup Google Test using FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  LoadBalancerTests
  tests/LoadBalancerTests.cpp
)
target_link_libraries(
  LoadBalancerTests
  PRIVATE LBLibrary gtest_main
)

include(GoogleTest)
gtest_discover_tests(LoadBalancerTests)
