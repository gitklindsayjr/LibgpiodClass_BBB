 # set minimum cmake version
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# project name and language
project(CMake_GpiodTest LANGUAGES CXX)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_FIND_ROOT_PATH  /usr/bin /usr/arm-linux-gnueabihf)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Conditional allowed cross-compiling on a Ubuntu PC and NATIVELY on a Beaglebone Black
# Debian 13, trixie - kernel version 6.12.49-bone36 Sep 25 19:56:57 UTC 2025 armv7l GNU/Linux
# The libgpiod library and include files were copied from the Beaglebone to the Ubuntu PC
# directory: ~/MicroControllers/Gpiod-Lib
if(NATIVE)
	set(EXTERNAL_LIB_DIR "/usr")
	set(EXTERNAL_SHARED_LIB_DIR "/usr/lib/arm-linux-gnueabihf")
	set(EXTERNAL_INCLUDE_DIR "${EXTERNAL_LIB_DIR}/include")
	set(GPIOD_LIB "/usr/lib/arm-linux-gnueabihf/libgpiod.so")
	set(CMAKE_CXX_FLAGS "-DNATIVE -std=c++14 -fmessage-length=0 -fpic -Wall")
else()
	set(EXTERNAL_LIB_DIR "~/MicroControllers/Gpiod-Lib")
	set(EXTERNAL_SHARED_LIB_DIR "${EXTERNAL_LIB_DIR}/lib/arm-linux-gnueabihf")
	set(EXTERNAL_INCLUDE_DIR "${EXTERNAL_LIB_DIR}/include")
	set(GPIOD_LIB "${EXTERNAL_SHARED_LIB_DIR}/libgpiod.so")
	set(CMAKE_CXX_FLAGS "-std=c++14 -fmessage-length=0 -fPIC -Wall")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "External library: ${EXTERNAL_SHARED_LIB_DIR}")
message(STATUS "C++ compiler flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ flags, Debug configuration: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags, Release configuration: ${CMAKE_CXX_FLAGS_RELEASE}")

# Define source files
set(SOURCE_FILES
	src/main.cpp
	src/BBB_Gpiod.cpp
	src/gpiod_class.cpp
)
set(INCLUDE_FILES
	src/BBB_Gpiod.h
	src/gpiod_class.h
	src/gpiod_structs.h
)
message(STATUS "Source path: ${PROJECT_NAME}")
message(STATUS "Source files: ${SOURCE_FILES}")

set(TARGET_NAME "gpiod-test")
# Add a executable target
add_executable(${TARGET_NAME} ${SOURCE_FILES})

# For compilation include other files
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Assuming headers are in ${EXTERNAL_INCLUDE_DIR}
target_include_directories(${TARGET_NAME} PUBLIC ${EXTERNAL_INCLUDE_DIR})

# Link the executable against the imported external library
target_link_libraries(${TARGET_NAME} PUBLIC ${GPIOD_LIB})


