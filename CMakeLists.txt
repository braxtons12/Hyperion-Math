cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include(FetchContent)

project(HyperionMath VERSION 0.1.0)

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
# Import Hyperion-Utils
#############################################################################
FetchContent_Declare(HyperionUtils
	GIT_REPOSITORY "https://github.com/braxtons12/Hyperion-Utils"
	)

FetchContent_MakeAvailable(HyperionUtils)
#############################################################################
#############################################################################

add_library(HyperionMath INTERFACE)

if(MSVC)
	target_compile_options(HyperionMath INTERFACE /WX /W4 /std:c++20)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "clang")
	if(NOT WIN32)
		target_compile_options(HyperionMath INTERFACE
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
			)
	else()
		target_compile_options(HyperionMath INTERFACE
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
			)
	endif()
else()
	target_compile_options(HyperionMath INTERFACE
		-std=c++20
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-c++98-c++11-c++14-compat-pedantic
		-Wno-c++20-compat
		)
endif()

target_include_directories(HyperionMath BEFORE INTERFACE
	"${CMAKE_SOURCE_DIR}/include"
	)

###### We add headers to sources sets because it helps with `#include` lookup for some tooling #####

set(EXPORTS
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/Exponentials.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/General.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/Interpolator.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/Point2.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/Point3.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/Random.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/TrigFuncs.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/Vec2.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/Vec3.h"
	"${CMAKE_SOURCE_DIR}/include/HyperionMath/HyperionMath.h"
	)

target_sources(HyperionMath INTERFACE
	${EXPORTS}
	)

target_link_libraries(HyperionMath INTERFACE
	GSL
	HyperionUtils
	)
