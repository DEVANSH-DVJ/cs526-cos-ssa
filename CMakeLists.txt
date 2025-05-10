cmake_minimum_required(VERSION 3.15)
project(CoS-SSA LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Ensure directories for generated files exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/src/cfg ${CMAKE_BINARY_DIR}/src/ddg ${CMAKE_BINARY_DIR}/src/ssa ${CMAKE_BINARY_DIR}/src/llvm)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/cfg ${CMAKE_BINARY_DIR}/include/ddg ${CMAKE_BINARY_DIR}/include/ssa ${CMAKE_BINARY_DIR}/include/llvm)

# Public headers
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/cfg)
include_directories(${PROJECT_SOURCE_DIR}/include/ddg)
include_directories(${PROJECT_SOURCE_DIR}/include/ssa)
include_directories(${PROJECT_SOURCE_DIR}/include/llvm)

# Include generated parser headers
include_directories(${CMAKE_BINARY_DIR}/include/cfg)
include_directories(${CMAKE_BINARY_DIR}/include/ddg)
include_directories(${CMAKE_BINARY_DIR}/include/ssa)
include_directories(${CMAKE_BINARY_DIR}/include/llvm)

# Flex & Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(LLVM REQUIRED CONFIG)

# CFG parser: generate source + defines header in build/include/cfg
BISON_TARGET(CFG_PARSER
    ${PROJECT_SOURCE_DIR}/include/cfg/cfg.y
    ${CMAKE_BINARY_DIR}/src/cfg/cfg.tab.cc
    DEFINES_FILE ${CMAKE_BINARY_DIR}/include/cfg/cfg.tab.hh
)
FLEX_TARGET(CFG_SCANNER
    ${PROJECT_SOURCE_DIR}/include/cfg/cfg.l
    ${CMAKE_BINARY_DIR}/src/cfg/cfg.scan.cc
    COMPILE_FLAGS --yylineno
)

# SSA parser: generate source + defines header in build/include/ssa
BISON_TARGET(SSA_PARSER
    ${PROJECT_SOURCE_DIR}/include/ssa/ssa.y
    ${CMAKE_BINARY_DIR}/src/ssa/ssa.tab.cc
    DEFINES_FILE ${CMAKE_BINARY_DIR}/include/ssa/ssa.tab.hh
)
FLEX_TARGET(SSA_SCANNER
    ${PROJECT_SOURCE_DIR}/include/ssa/ssa.l
    ${CMAKE_BINARY_DIR}/src/ssa/ssa.scan.cc
    COMPILE_FLAGS --yylineno
)

# Collect implementation files
file(GLOB_RECURSE CORE_SRC
    src/*.cc
)

# Exclude generated parser sources
list(REMOVE_ITEM CORE_SRC
    src/cfg/cfg.scan.cc
    src/cfg/cfg.tab.cc
    src/ssa/ssa.scan.cc
    src/ssa/ssa.tab.cc
)

# Add generated parser sources
list(APPEND CORE_SRC
    ${BISON_CFG_PARSER_OUTPUTS}
    ${FLEX_CFG_SCANNER_OUTPUTS}
    ${BISON_SSA_PARSER_OUTPUTS}
    ${FLEX_SSA_SCANNER_OUTPUTS}
)

# Create library
add_library(cos_ssa_lib ${CORE_SRC})
target_include_directories(cos_ssa_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

if(LLVM_FOUND)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    llvm_map_components_to_libnames(LLVM_LIBRARIES support core irreader)

    # Main executable
    add_executable(cos_ssa src/main.cc)
    target_link_libraries(cos_ssa PRIVATE cos_ssa_lib ${LLVM_LIBRARIES})
else()
    message(FATAL_ERROR "LLVM not found.")
endif()

# Strip binary on Release
string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)

if(CMAKE_BUILD_TYPE_UPPER STREQUAL "RELEASE")
    add_custom_command(TARGET cos_ssa POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:cos_ssa>
    )
endif()
