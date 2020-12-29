include_directories(${openfst_SOURCE_DIR}/src/include)

set(kaldilm_srcs
  arpa_file_parser.cc
  arpa_lm_compiler.cc
  string_utils.cc
)

add_library(kaldilm_core SHARED ${kaldilm_srcs})
target_link_libraries(kaldilm_core fst)

add_executable(arpa_file_parser_test arpa_file_parser_test.cc)
target_link_libraries(arpa_file_parser_test kaldilm_core)

add_executable(arpa_lm_compiler_test arpa_lm_compiler_test.cc)
target_link_libraries(arpa_lm_compiler_test kaldilm_core)
target_compile_definitions(arpa_lm_compiler_test  PRIVATE KALDILM_TEST_DATA_DIR=${CMAKE_CURRENT_LIST_DIR})
