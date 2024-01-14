cmake_minimum_required(VERSION 3.16)

project(WIKIFIX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)
find_package(CURL REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(pegtl REQUIRED)

add_subdirectory(src)
set_target_properties(wikifix PROPERTIES CXX_STANDARD 20)
target_link_libraries(wikifix CURL::libcurl)
target_link_libraries(wikifix jsoncpp_lib)
target_link_libraries(wikifix ${Boost_LIBRARIES})
target_link_libraries(wikifix taocpp::pegtl)
