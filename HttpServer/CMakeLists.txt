set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

aux_source_directory(. SOURCE_FILES)

list(APPEND SOURCE_FILES ${SOURCE_FILES_HELPER} HttpConfig)

add_executable(httpserver ${SOURCE_FILES})
target_link_libraries(httpserver zhs log4cxx crypto++ ${CMAKE_THREAD_LIBS_INIT} ${LIBXML2_LIBRARIES} ${ZLIB_LIBRARIES} ${LUA_LIBRARIES} luabindd)