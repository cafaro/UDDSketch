include_directories(${PROJECT_SOURCE_DIR}/libuddsketch/include)
link_directories(${PROJECT_SOURCE_DIR}/libs)
set(SOURCES main.c)

add_executable(uddsketch-test ${SOURCES})

target_link_libraries(uddsketch-test uddsketch)

