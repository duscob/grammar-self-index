cmake_minimum_required(VERSION 3.7)
project(grammar-self-index)

#Global Setup
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")


# Location of internal cmake modules
set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/cmake)

# Guard against in-source builds and bad build-type strings
include(ConfigSafeGuards)


##Testing
#include(CTest)
##set(LIBS string_dictionaries)
#
#find_package(Threads REQUIRED)
#
#find_package(GTest)
#if (GTEST_FOUND)
#    set(LIBS ${LIBS} GTest::Main)
#else ()
#    include(ConfigGTest)
#    set(LIBS ${LIBS} gtest gtest_main)
#endif ()


# Setup dependencies
find_package(sdsl 3.0)
if (NOT sdsl_FOUND)
    include(ConfigSDSL)
endif ()

find_library(CDS_LIB cds)
if (NOT CDS_LIB)
    include(ConfigCDS)
    set(CDS_LIB cds::cds)
endif ()
set(LIBS ${LIBS} ${CDS_LIB})

find_package(benchmark 1.6)
if (NOT benchmark_FOUND)
    include(ConfigGBench)
endif ()
list(APPEND LIBS benchmark::benchmark benchmark::benchmark_main)

# Set common include folder for module
set(COMMON_INCLUDES
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_INSTALL_PREFIX}/include
        ${CMAKE_PREFIX_PATH}/include)
include_directories(${COMMON_INCLUDES})


# Build targets
set(SLP_SAMPLING
        utils/Samplings/binarynode.h
        utils/Samplings/CSD.cpp utils/Samplings/CSD.h
        utils/Samplings/CSD_HTFC.cpp utils/Samplings/CSD_HTFC.h
        utils/Samplings/hutucker2.h
        utils/Samplings/sampling.cpp utils/Samplings/sampling.h
        utils/Samplings/VByte2.cpp utils/Samplings/VByte2.h

        utils/Utils/deltacodes.cpp utils/Utils/deltacodes.h
        utils/Utils/offsets.h
        utils/Utils/SuffixArray.cpp utils/Utils/SuffixArray.h
        utils/memory/mem_monitor/mem_monitor.hpp

        SelfGrammarIndex.cpp SelfGrammarIndex.h
        SelfGrammarIndexBS.cpp SelfGrammarIndexBS.h
        SelfGrammarIndexPT.cpp SelfGrammarIndexPT.h
        SelfGrammarIndexPTS.cpp SelfGrammarIndexPTS.h

        ################REPAIR FILES#########################
        utils/repair/arrayG.cpp utils/repair/arrayG.h
        utils/repair/basics.cpp utils/repair/basics.h
        utils/repair/dictionary.cpp utils/repair/dictionary.h
        utils/repair/records.cpp utils/repair/records.h
        utils/repair/heap.cpp utils/repair/heap.h
        utils/repair/hash.cpp utils/repair/hash.h
        utils/repair/RePair.cpp utils/repair/RePair.h
        utils/build_hyb_lz77.h
        ################REPAIR FILES#########################
        binary_relation.cpp binary_relation.h
        compressed_grammar.cpp compressed_grammar.h
        trees/dfuds_tree.cpp trees/dfuds_tree.h
        trees/bp_tree.cpp trees/bp_tree.h
        trees/patricia_tree/compact_patricia_tree.cpp trees/patricia_tree/compact_patricia_tree.h
        trees/patricia_tree/patricia_tree.cpp trees/patricia_tree/patricia_tree.h
        trees/patricia_tree/sampled_patrica_tree.h trees/patricia_tree/sampled_patrica_tree.cpp
        trees/trie/compact_trie.cpp trees/trie/compact_trie.h
        trees/trie/Trie.cpp trees/trie/Trie.h
        utils/grammar.cpp utils/grammar.h
#        tests/collections.cpp
        bench/repetitive_collections.h
        SelfGrammarIndexBSQ.cpp SelfGrammarIndexBSQ.h
        utils/CLogger.cpp utils/CLogger.h
        )

set(SOURCE_FILES

        SelfGrammarIndex.cpp SelfGrammarIndex.h
        SelfGrammarIndexBS.cpp SelfGrammarIndexBS.h
        SelfGrammarIndexPT.cpp SelfGrammarIndexPT.h
        SelfGrammarIndexPTS.cpp SelfGrammarIndexPTS.h

        ################REPAIR FILES#########################
        utils/repair/arrayG.cpp utils/repair/arrayG.h
        utils/repair/basics.cpp utils/repair/basics.h
        utils/repair/dictionary.cpp utils/repair/dictionary.h
        utils/repair/records.cpp utils/repair/records.h
        utils/repair/heap.cpp utils/repair/heap.h
        utils/repair/hash.cpp utils/repair/hash.h
        utils/repair/RePair.cpp utils/repair/RePair.h
        #utils/build_hyb_lz77.h
        ################REPAIR FILES#########################

        binary_relation.cpp binary_relation.h
        compressed_grammar.cpp compressed_grammar.h

        trees/dfuds_tree.cpp trees/dfuds_tree.h
        trees/bp_tree.cpp trees/bp_tree.h

        trees/patricia_tree/compact_patricia_tree.cpp trees/patricia_tree/compact_patricia_tree.h
        trees/patricia_tree/patricia_tree.cpp trees/patricia_tree/patricia_tree.h
        trees/patricia_tree/sampled_patrica_tree.h trees/patricia_tree/sampled_patrica_tree.cpp

        trees/trie/compact_trie.cpp trees/trie/compact_trie.h
        trees/trie/Trie.cpp trees/trie/Trie.h

        utils/grammar.cpp utils/grammar.h
        utils/memory/mem_monitor/mem_monitor.hpp
        utils/CLogger.cpp utils/CLogger.h
#        tests/collections.cpp
        bench/repetitive_collections.h

        SelfGrammarIndexBSQ.cpp SelfGrammarIndexBSQ.h
        simple_grid.cpp simple_grid.h
        macros.h
#        utils/qgram/qgram_sampling.cpp
#        utils/qgram/qgram_sampling.h
        )

option(USE_MEM_MONITOR "Enter mem-monitor mode" OFF)
if (USE_MEM_MONITOR STREQUAL ON)
    add_definitions(-DMEM_MONITOR)
else()
    remove_definitions(-DMEM_MONITOR)
endif()

option(USE_PRINT_MODE "Enter print mode" OFF)
if (USE_PRINT_MODE STREQUAL ON)
    add_definitions(-DPRINT_LOGS)
else()
    remove_definitions(-DPRINT_LOGS)
endif()

option(BUILD_EXTERNAL_INDEXES "Enter print mode" OFF)
if (BUILD_EXTERNAL_INDEXES STREQUAL ON)
    add_definitions(-DBUILD_EXTERNAL_INDEXES)
else()
    remove_definitions(-DBUILD_EXTERNAL_INDEXES)
endif()

#add_executable(improved_grammar_index main.cpp ${SOURCE_FILES} )

include(internal_utils)

#cxx_executable_with_flags(binary_relation_performance "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/binary_relation_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(grammar_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/utils/grammar_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(compressed_grammar_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/compressed_grammar_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(patricia_tree_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/utils/patricia_tree_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(compact_trie_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/compact_trie_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(self_index_bin_search "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/self_index_bin_search.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(self_index_pt "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/self_index_PT.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(wavelet_tree_ap "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/wavelet_tree_ap.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(wavelet_tree_range "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/wt_range_search.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(building_csa_lcp_rmq "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/building_csa_lcp_rmq.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(suffixes_grammar "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/suffixes_grammar.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(repair_grammar_bench "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/repair_grammar_bench.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(breakdown_space "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/breakdown_space.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(pattern_length_bench "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/pattern_length_bench.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(idx_locate_performance "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/idx_locate_performance.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(locate_div "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/locate_div.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(extract_div "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/extract_div.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(idx_extract_performance "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/idx_extract_performance.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(expand_rule_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/expand_rule_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(compare_rule_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/compare_rules_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(display_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/display_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(locate_bs_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/locate_bs_test.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(count_bitevecto_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" tests/count_bitvector_test.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(slp_sampling_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" tests/slp_sampling_test.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(2d_generation "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" tests/2d_generation.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_z_compression "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_Z_compression.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_expand_rule "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_expand_rule.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_compare_rules "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_compare_rules.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_bp_tree "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_bp_tree.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_dfuds_tree "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_dfuds_tree.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_locate_bs "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_locate_bs.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_pattern_len_display "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_pattern_len_display.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_pattern_len_locate "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_pattern_len_locate.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_qgram_locate    "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_qgram_locate.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(bench_qgram_cmp_rules       "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_qgram_cmp_rules.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(bench_find_ranges_pattern_len "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_find_ranges_pattern_len.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_find_second_occ "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_find_second_occ.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(measures "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/measures.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_break_down_space "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_break_down_space.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_sl_int_vector "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/bench_sl_int_vector.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(create_files_patterns "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/create_files_patterns.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(grammar_benchmark "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/grammar_benchmark.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(binary_relation_benchmark "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/binary_relation_benchmark.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(selfgrammarnindex_benchmark "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/selfgrammarindex_bench.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(slp_benchmark "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/slp_benchmark.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(idx_build_lz "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" bench/lz_build.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(idx_build_rlcsa "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;rmqrmmBP;rlcsa" bench/idx_build_rlcsa.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(rlcsa_locate "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;rmqrmmBP;rlcsa" bench/rlcsa_locate.cpp ${SOURCE_FILES})


#cxx_executable_with_flags(bench_pattern_len_other_locate "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_pattern_len_other_locate.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_pattern_len_other_display "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_pattern_len_other_display.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_qgram_idx_build "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_qgram_idx_build.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(bench_qgram_locate_pattern_len    "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_qgram_locate_pattern_len.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(bench_qgram_display_pattern_len    "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_qgram_display_pattern_len.cpp ${SLP_SAMPLING} )
#cxx_executable_with_flags(locate_other "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/bench_locate.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(idx_build "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/idx_build.cpp ${SOURCE_FILES})

#cxx_executable_with_flags(bench_pattern_len_other_locate "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds" bench/bench_pattern_len_other_locate.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_pattern_len_other_display "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds" bench/bench_pattern_len_other_display.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(bench_qgram_idx_build "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;cds" bench/bench_qgram_idx_build.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(bench_qgram_locate_pattern_len    "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64;cds" bench/bench_qgram_locate_pattern_len.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(bench_qgram_display_pattern_len    "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds" bench/bench_qgram_display_pattern_len.cpp ${SLP_SAMPLING} )
#cxx_executable_with_flags(locate_other "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds" bench/bench_locate.cpp ${SOURCE_FILES})
#cxx_executable_with_flags(idx_build "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds" bench/idx_build.cpp ${SOURCE_FILES})




#cxx_executable_with_flags(bench_locate_pattern_len    "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds" bench/bench_qgram_locate_pattern_len.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(bench_display_pattern_len    "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds" bench/bench_qgram_display_pattern_len.cpp ${SLP_SAMPLING} )


#cxx_executable_with_flags(build_indices "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds::cds" bench/build_indices.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(process_coll "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds::cds" bench/bench_collections.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(locate_indices "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds::cds" bench/locate_indices.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(display_indices "" "${LIBS};benchmark;slp;sdsl;divsufsort;divsufsort64;cds::cds" bench/display_indices.cpp ${SLP_SAMPLING})
#cxx_executable_with_flags(space "" "${LIBS};benchmark;sdsl;slp;divsufsort;divsufsort64;cds::cds" bench/bench_space.cpp ${SLP_SAMPLING})



if (NOT INV_PI_T)
    set(INV_PI_T 8)
endif()
message("INV_PI_T ${INV_PI_T}")

if (NOT INV_PI_T_TRIE)
    set(INV_PI_T_TRIE 8)
endif()
message("INV_PI_T_TRIE ${INV_PI_T_TRIE}")

if (NOT INV_PI_T_QGRAM)
    set(INV_PI_T_QGRAM 8)
endif()
message("INV_PI_T_QGRAM ${INV_PI_T_QGRAM}")

message("MACROS ${PROJECT_SOURCE_DIR}/config_macros.h.in")

configure_file(
        "${PROJECT_SOURCE_DIR}/config_macros.h.in"
        "${PROJECT_SOURCE_DIR}/macros.h"
)


#target_link_libraries(improved_grammar_index sdsl)
#target_link_libraries(improved_grammar_index cds)
#target_link_libraries(improved_grammar_index divsufsort)
#target_link_libraries(improved_grammar_index divsufsort64)
#target_link_libraries(improved_grammar_index slp)

#target_link_libraries(improved_grammar_index rmqrmmBP)
#target_link_libraries(improved_grammar_index hsi)
#target_link_libraries(improved_grammar_index rlcsa)
#target_link_libraries(improved_grammar_index lzend)


# Build benchmarks
#TODO Verify these compilation flags
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -fstrict-aliasing -DNDEBUG -march=native -fopenmp")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -ggdb -Ofast -fstrict-aliasing -march=native")

# Always link with libstdc++fs.a (std::filesystem) when using GCC 8.
# Note: This command makes sure that this option comes pretty late on the cmdline.
link_libraries("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:-lstdc++fs>")
add_link_options(LINKER:--as-needed)

set(G_INDEX_PTS_SOURCE_FILES
        utils/Samplings/VByte2.cpp utils/Samplings/VByte2.h

        utils/Utils/deltacodes.cpp utils/Utils/deltacodes.h
        utils/Utils/offsets.h
        utils/Utils/SuffixArray.cpp utils/Utils/SuffixArray.h
        utils/memory/mem_monitor/mem_monitor.hpp

        SelfGrammarIndex.cpp SelfGrammarIndex.h
        SelfGrammarIndexBS.cpp SelfGrammarIndexBS.h
        SelfGrammarIndexPT.cpp SelfGrammarIndexPT.h
        SelfGrammarIndexPTS.cpp SelfGrammarIndexPTS.h

        ################REPAIR FILES#########################
        utils/repair/arrayG.cpp utils/repair/arrayG.h
        utils/repair/basics.cpp utils/repair/basics.h
        utils/repair/dictionary.cpp utils/repair/dictionary.h
        utils/repair/records.cpp utils/repair/records.h
        utils/repair/heap.cpp utils/repair/heap.h
        utils/repair/hash.cpp utils/repair/hash.h
        utils/repair/RePair.cpp utils/repair/RePair.h
        utils/build_hyb_lz77.h
        ################REPAIR FILES#########################

        binary_relation.cpp binary_relation.h
        compressed_grammar.cpp compressed_grammar.h
        trees/dfuds_tree.cpp trees/dfuds_tree.h
        trees/bp_tree.cpp trees/bp_tree.h
        trees/patricia_tree/compact_patricia_tree.cpp trees/patricia_tree/compact_patricia_tree.h
        trees/patricia_tree/patricia_tree.cpp trees/patricia_tree/patricia_tree.h
        trees/patricia_tree/sampled_patrica_tree.h trees/patricia_tree/sampled_patrica_tree.cpp
        trees/trie/compact_trie.cpp trees/trie/compact_trie.h
        trees/trie/Trie.cpp trees/trie/Trie.h
        utils/grammar.cpp utils/grammar.h
        #        tests/collections.cpp
        bench/repetitive_collections.h
        #        SelfGrammarIndexBSQ.cpp SelfGrammarIndexBSQ.h
        utils/CLogger.cpp utils/CLogger.h
        )

include(ConfigSRIBenchmark)

find_library(GFLAGS_LIB gflags REQUIRED)
find_package(Threads)

add_executable(bm_build_items bench/bm_build_items.cpp ${G_INDEX_PTS_SOURCE_FILES})
target_link_libraries(bm_build_items "${GFLAGS_LIB};${LIBS}")

add_executable(bm_locate bench/bm_locate.cpp ${G_INDEX_PTS_SOURCE_FILES})
target_link_libraries(bm_locate "${GFLAGS_LIB};${LIBS}")
