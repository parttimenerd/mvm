cmake_minimum_required(VERSION 2.8.4)
project(mvm)


## Configure compiler and libraries : Boost
add_compile_options(-std=c++1y -Wall -Wextra)
include_directories(${PROJECT_SOURCE_DIR}/include)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

  add_compile_options(-fdiagnostics-color=auto)  
  #  set(Boost_LIBRARY_DIR $ENV{BOOST_GCC_LIB}) # if set manually

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

  add_compile_options(-stdlib=libc++)
  #set(Boost_LIBRARY_DIR $ENV{BOOST_CLANG_LIB}) # if set manually

endif()

# ref to add libs: https://sites.google.com/site/boostjp/tips/build_link
#find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})


## Generate unit tests in ./test
#enable_testing()

#file(GLOB test_sources RELATIVE ${PROJECT_SOURCE_DIR} test/*.cpp)

#foreach(path IN LISTS test_sources)

#  get_filename_component(target ${path} NAME_WE)
#  add_executable(${target} ${path})
#  add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
#  set_property(TEST ${target} APPEND PROPERTY LABELS ${path})

#  TARGET_LINK_LIBRARIES(${target} ${Boost_LIBRARIES})

#  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # link libc++
    #    TARGET_LINK_LIBRARIES(${target} c++)
    #  endif()
  
    #endforeach()


## Generate executables in ./src
file(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} src/*.cpp)

file(GLOB implementation_headers "src/*.hpp")

add_library(mvm
	${implementation_headers}
)

target_link_libraries(mvm)

foreach(path IN LISTS sources)

  get_filename_component(target ${path} NAME_WE)
  add_executable(${target} ${path})  

  TARGET_LINK_LIBRARIES(${target} ${Boost_LIBRARIES})

  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # link libc++
    TARGET_LINK_LIBRARIES(${target} c++)
  endif()

endforeach()
