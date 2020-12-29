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
