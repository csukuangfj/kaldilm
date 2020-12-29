
set(kaldilm_srcs
  arpa_file_parser.cc
  string_utils.cc
  symbol_table.cc
)

add_library(kaldilm_core SHARED ${kaldilm_srcs})

add_executable(symbol_table_test symbol_table_test.cc)
target_link_libraries(symbol_table_test kaldilm_core)
