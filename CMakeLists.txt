cmake_minimum_required(VERSION 3.14)

project(Main)

enable_testing()
add_subdirectory(unit_tests/tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

option(ENABLE_LOGGING "Enable logging" OFF)
option(DEBUG "Debug version" OFF)
option(ENABLE_BIG_TESTS "Enables big data tests" OFF)

set(RELEASE_COMPILE_OPTIONS
	-O2
	-fstack-protector
	-fPIE
	-fstrict-overflow
	-Wformat-security
	-Wformat=2
	-Wall
	-Wextra
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wcast-align
	-Wpointer-arith
	-Wunreachable-code
	-Wno-pre-c++17-compat
)

set(DEBUG_COMPILE_OPTIONS
	-D_DEBUG
	-ggdb3
	-O0
	-Wall
	-Wextra
	-Weffc++
	-Wc++14-compat
	-Wmissing-declarations
	-Wcast-align
	-Wcast-qual
	-Wchar-subscripts
	-Wconversion
	-Wctor-dtor-privacy
	-Wempty-body
	-Wfloat-equal
	-Wformat-security
	-Winline
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wpacked
	-Wpointer-arith
	-Winit-self
	-Wredundant-decls
	-Wshadow
	-Wsign-conversion
	-Wsign-promo
	-Wstrict-overflow=2
	-Wsuggest-override
	-Wswitch-default
	-Wswitch-enum
	-Wundef
	-Wunreachable-code
	-Wunused
	-Wvariadic-macros
	-Wno-missing-field-initializers
	-Wno-narrowing
	-Wno-old-style-cast
	-Wno-varargs
	-Wstack-protector
	-fcheck-new
	-fsized-deallocation
	-fstack-protector
	-fstrict-overflow
	-fno-omit-frame-pointer
	-Wlarger-than=8192
	-fPIE
	-Werror=vla
	-Wno-pre-c++17-compat
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

# ----- range_queries library -----

add_library(range_queries STATIC
	src/range_queries.cpp)

target_include_directories(range_queries PRIVATE
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/RB_Tree/include)

if(ENABLE_LOGGING)
	target_compile_definitions(range_queries PRIVATE ENABLE_LOGGING)
endif()

if(DEBUG)
	target_compile_options(range_queries PRIVATE ${DEBUG_COMPILE_OPTIONS})
	target_compile_definitions(range_queries PRIVATE DEBUG)

	target_link_options(range_queries PRIVATE
		-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
	)
else()
	target_compile_options(range_queries PRIVATE ${RELEASE_COMPILE_OPTIONS})
endif()

# ----- executable -----

add_executable(range_queries.x ./main.cpp)
target_include_directories(range_queries.x PRIVATE
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/RB_Tree/include)

if(ENABLE_LOGGING)
	target_compile_definitions(range_queries.x PRIVATE ENABLE_LOGGING)
endif()

if(DEBUG)
	target_compile_definitions(range_queries.x PRIVATE DEBUG)
	target_compile_options(range_queries.x PRIVATE ${DEBUG_COMPILE_OPTIONS})

	target_link_options(range_queries.x PRIVATE
		-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
	)
else()
	target_compile_options(range_queries.x PRIVATE ${RELEASE_COMPILE_OPTIONS})
endif()

target_link_libraries(range_queries.x PRIVATE range_queries)
