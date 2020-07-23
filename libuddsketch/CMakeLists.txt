include_directories(${PROJECT_SOURCE_DIR}/libuddsketch/include ${PROJECT_SOURCE_DIR}/libuddsketch/include/dict)
set(SOURCES
        src/dds_store.c
        src/dds_bucket_id_mapping.c
        src/dds_sketch.c
        src/dds_gsketch.c
        src/dds_mapstore.c
        src/dds_psketch.c
        src/dict/hashtable_common.c
        src/dict/hashtable2.c
        src/dict/hashtable.c
        src/dict/dict.c
        src/dict/hb_tree.c
        src/dict/pr_tree.c
        src/dict/rb_tree.c
        src/dict/skiplist.c
        src/dict/sp_tree.c
        src/dict/tr_tree.c
        src/dict/tree_common.c
        src/dict/wb_tree.c)

add_library(uddsketch STATIC ${SOURCES})
target_link_libraries(uddsketch m)

install(TARGETS uddsketch DESTINATION libs)
