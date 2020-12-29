include_directories(${openfst_SOURCE_DIR}/src/include)

set(kaldilm_srcs
  arpa_file_parser.cc
  string_utils.cc
)

add_library(kaldilm_core SHARED ${kaldilm_srcs})
target_link_libraries(kaldilm_core fst)
