include_directories(${WIKIFIX_SOURCE_DIR}/include)

file(GLOB WIKIFIX_SRC "*")

add_executable(wikifix ${WIKIFIX_SRC})
set_target_properties(wikifix PROPERTIES CXX_STANDARD 23)
target_link_libraries(wikifix PRIVATE cpr::cpr jsoncpp_lib CLI11::CLI11)
