cmake_minimum_required(VERSION 3.16)

if (CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

project(WIKIFIX)

set(CPR_USE_SYSTEM_CURL "ON")

include(cmake/CPM.cmake)
CPMAddPackage("gh:CLIUtils/CLI11@2.3.2")
CPMAddPackage("gh:libcpr/cpr#master")
CPMAddPackage("gh:open-source-parsers/jsoncpp#1.9.5")

include_directories("include")
file(GLOB_RECURSE WIKIFIX_SRC "*.cc")
add_executable(wikifix "${WIKIFIX_SRC}")
set_target_properties(wikifix PROPERTIES CXX_STANDARD "23")
target_link_libraries(wikifix PRIVATE
  cpr::cpr
  jsoncpp_lib
  CLI11::CLI11)
