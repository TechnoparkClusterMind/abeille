cmake_minimum_required(VERSION 3.18)

project(abeille)

option(ENABLE_TESTS "Enable tests" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck" OFF)
option(ENABLE_COVERAGE "Enable coverage" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

add_compile_options(-Wpedantic -Wall -Wextra -Wshadow)
add_compile_options(-Wno-unused-parameter -Wno-unused-function)

if(ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()

if(ENABLE_CPPCHECK)
find_program(CMAKE_CXX_CPPCHECK cppcheck)
  if(CMAKE_CXX_CPPCHECK)
    list(
      APPEND CMAKE_CXX_CPPCHECK
      "--enable=all" "--quiet"
      "--suppress=missingInclude" "--suppress=unusedFunction"
      "lib"
    )
      
    add_custom_target(cppcheck ALL DEPENDS ${PROJECT_NAME}
      COMMAND ${CMAKE_CXX_CPPCHECK}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Static code analysis using cppcheck"
    )
  endif()
endif()

if(ENABLE_COVERAGE)
  add_compile_options(--coverage -g -O0)
  add_link_options(--coverage)
endif()

if(ENABLE_SANITIZERS)
  add_compile_options(-fsanitize=leak)
  add_compile_options(-fsanitize=undefined)
  add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
  add_link_options(-fsanitize=leak)
  add_link_options(-fsanitize=undefined)
  add_link_options(-fsanitize=address)
elseif(NOT ENABLE_COVERAGE)
  add_compile_options("-O3")
endif()

# Common libraries for all executables
add_subdirectory(utils)
add_subdirectory(rpc_global)

# Three executables will be compiled
# * abeille-raft -- launch raft node
# * abeille-worker -- launch worker node
# * abeille-client -- launch the interface 
#                     for communication with the raft cluster
add_subdirectory(raft_node)
# add_subdirectory(worker_node)
add_subdirectory(user)


add_executable(main cmd/main.cpp)
target_link_libraries(main rpc_global rpc_raft utils)
