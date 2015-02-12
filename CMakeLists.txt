cmake_minimum_required(VERSION 2.8)
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

###############
### TARGETS ###
###############

# Collect executables in ./src
FILE(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} src/*.cpp)

FILE(GLOB library_sources RELATIVE ${PROJECT_SOURCE_DIR} src/*.cpp)
LIST(REMOVE_ITEM library_sources ${PROJECT_SOURCE_DIR} src/main.cpp)

###################
### mvm_library ###
###################

ADD_LIBRARY(mvm_library
	${library_sources}
)

SET_TARGET_PROPERTIES(mvm_library PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(mvm_library PROPERTIES OUTPUT_NAME mvm)

######################
### mvm_executable ###
######################

ADD_EXECUTABLE(mvm_executable
	${sources}
)

SET_TARGET_PROPERTIES(mvm_executable PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(mvm_executable PROPERTIES OUTPUT_NAME mvm)

###################
### END TARGETS ###
###################

# Clang specific libraries

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # link libc++
  TARGET_LINK_LIBRARIES(mvm_executable c++)
  TARGET_LINK_LIBRARIES(mvm_library c++)
ENDIF()
