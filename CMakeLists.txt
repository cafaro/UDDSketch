cmake_minimum_required(VERSION 3.15)
project(uddsketch C)

set(CMAKE_C_STANDARD 11)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "install" CACHE PATH "..." FORCE)
endif()

add_subdirectory(libuddsketch)
add_subdirectory(uddsketch-test)

install(FILES libuddsketch/include/dds_gsketch.h
        libuddsketch/include/dds_psketch.h
        libuddsketch/include/dds_store.h
        libuddsketch/include/dds_sketch.h
        libuddsketch/include/dds_bucket_id_mapping.h
        DESTINATION include)

