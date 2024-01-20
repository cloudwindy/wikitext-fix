include_directories(${WIKIFIX_SOURCE_DIR}/include)

file(GLOB WIKIFIX_SRC "*")

add_executable(wikifix ${WIKIFIX_SRC})
set_target_properties(wikifix PROPERTIES CXX_STANDARD 20)
target_link_libraries(wikifix jsoncpp_lib)
target_link_libraries(wikifix cpr::cpr)
target_link_libraries(wikifix ${Boost_LIBRARIES})
