cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
include(FetchContent)

project(HyperionUtilsTest VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_EXTENSIONS OFF)

#############################################################################
# Import Microsoft GSL Implementation
#############################################################################
FetchContent_Declare(GSL
	GIT_REPOSITORY "https://github.com/microsoft/GSL"
	GIT_TAG "v3.1.0"
	)

FetchContent_MakeAvailable(GSL)
#############################################################################
#############################################################################

#############################################################################
# Import GoogleTest
#############################################################################
FetchContent_Declare(googletest
	GIT_REPOSITORY "https://github.com/google/googletest"
	GIT_TAG "release-1.10.0"
	)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
#############################################################################
#############################################################################

#############################################################################
# Import Hyperion Utils
#############################################################################
FetchContent_Declare(HyperionMath
	GIT_REPOSITORY "https://github.com/braxtons12/Hyperion-Math"
	)

FetchContent_MakeAvailable(HyperionMath)
#############################################################################
#############################################################################

add_executable(HyperionMathTest "${CMAKE_SOURCE_DIR}/src/Test.cpp")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "clang" OR APPLE)
	set_target_properties(HyperionMathTest PROPERTIES CXX_CLANG_TIDY clang-tidy)
endif()

if(MSVC)
	target_compile_options(HyperionMathTest PRIVATE /WX /W4 /std:c++20)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "clang")
	if(NOT WIN32)
		target_compile_options(HyperionMathTest PRIVATE
			-std=c++20
			-Wall
			-Wextra
			-Wpedantic
			-Weverything
			-Werror
			-Wno-c++98-compat
			-Wno-c++98-compat-pedantic
			-Wno-c++98-c++11-c++14-compat-pedantic
			-Wno-c++20-compat
			-Wno-global-constructors
			)
	else()
		target_compile_options(HyperionMathTest PRIVATE
			-std=c++20
			-Wall
			-Wextra
			-Wpedantic
			-Weverything
			-Werror
			-Wno-c++98-compat
			-Wno-c++98-compat-pedantic
			-Wno-c++98-c++11-c++14-compat-pedantic
			-Wno-c++20-compat
			-Wno-global-constructors
			)
	endif()
else()
	target_compile_options(HyperionMathTest PRIVATE
		-std=c++20
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-c++98-c++11-c++14-compat-pedantic
		-Wno-c++20-compat
		-Wno-global-constructors
		)
endif()

target_include_directories(HyperionMathTest BEFORE INTERFACE
	"${CMAKE_SOURCE_DIR}/src"
	)

if(UNIX AND NOT APPLE)
	target_link_libraries(HyperionMathTest PRIVATE
		curl
		GSL
		gtest
		HyperionMath
		)
else()
	target_link_libraries(HyperionMathTest PRIVATE
		GSL
		gtest
		HyperionMath
		)
endif()
