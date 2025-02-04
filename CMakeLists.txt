cmake_minimum_required(VERSION 3.15)
project(CoS-SSA LANGUAGES CXX)

# Build type
option(BUILD_DEBUG "Enable debug build" ON)
if(BUILD_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(COMPILE_FLAGS "-O0 -ggdb3 -Wall")
else()
  set(CMAKE_BUILD_TYPE Release)
  set(COMPILE_FLAGS "-O3 -Wall")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(${COMPILE_FLAGS})

# Public headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Flex & Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# CFG parser
BISON_TARGET(CFG_PARSER include/cfg/cfg.y src/cfg/cfg_parser.cc)
FLEX_TARGET(CFG_SCANNER include/cfg/cfg.l src/cfg/cfg_scanner.cc)

# SSA parser
BISON_TARGET(SSA_PARSER include/ssa/ssa.y src/ssa/ssa_parser.cc)
FLEX_TARGET(SSA_SCANNER include/ssa/ssa.l src/ssa/ssa_scanner.cc)

# Collect implementation files (excluding generated)
file(GLOB_RECURSE CORE_SRC
    src/*.cc
)
list(REMOVE_ITEM CORE_SRC
    src/cfg/cfg_parser.cc
    src/cfg/cfg_scanner.cc
    src/ssa/ssa_parser.cc
    src/ssa/ssa_scanner.cc
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
target_include_directories(cos_ssa_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Main executable
add_executable(cos_ssa src/main.cc)
target_link_libraries(cos_ssa PRIVATE cos_ssa_lib)

# Link argp, flex, bison, lex/yacc libraries
find_library(ARGP_LIB argp)
if(ARGP_LIB)
  target_link_libraries(cos_ssa PRIVATE ${ARGP_LIB})
endif()
find_library(FLEX_LIB fl)
if(FLEX_LIB)
  target_link_libraries(cos_ssa PRIVATE ${FLEX_LIB})
endif()
find_library(Y_LIB y)
if(Y_LIB)
  target_link_libraries(cos_ssa PRIVATE ${Y_LIB})
endif()

# Strip binary on Release
if(NOT BUILD_DEBUG)
  add_custom_command(TARGET cos_ssa POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:cos_ssa>
  )
endif()
