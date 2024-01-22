cmake_minimum_required(VERSION 3.16)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

project(WIKIFIX)

include(cmake/CPM.cmake)
CPMAddPackage("gh:CLIUtils/CLI11@2.3.2")
CPMAddPackage("gh:libcpr/cpr#1.10.5")
CPMAddPackage("gh:open-source-parsers/jsoncpp#1.9.5")

add_subdirectory(src)
