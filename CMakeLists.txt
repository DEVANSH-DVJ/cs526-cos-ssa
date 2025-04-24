cmake_minimum_required(VERSION 3.15)
project(CoS-SSA LANGUAGES CXX)

# Build type
option(BUILD_DEBUG "Enable debug build" ON)
if(BUILD_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(COMPILE_FLAGS "-O0")
else()
  set(CMAKE_BUILD_TYPE Release)
  set(COMPILE_FLAGS "-O3")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(${COMPILE_FLAGS})

# Ensure directories for generated files exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/src/cfg ${CMAKE_BINARY_DIR}/src/ddg ${CMAKE_BINARY_DIR}/src/ssa)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/cfg ${CMAKE_BINARY_DIR}/include/ddg ${CMAKE_BINARY_DIR}/include/ssa)

# Public headers
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/cfg)
include_directories(${PROJECT_SOURCE_DIR}/include/ddg)
include_directories(${PROJECT_SOURCE_DIR}/include/ssa)

# Include generated parser headers
include_directories(${CMAKE_BINARY_DIR}/include/cfg)
include_directories(${CMAKE_BINARY_DIR}/include/ddg)
include_directories(${CMAKE_BINARY_DIR}/include/ssa)

# Flex & Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

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

# Main executable
add_executable(cos_ssa src/main.cc)
target_link_libraries(cos_ssa PRIVATE cos_ssa_lib)

# Strip binary on Release
if(NOT BUILD_DEBUG)
  add_custom_command(TARGET cos_ssa POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:cos_ssa>
  )
endif()
