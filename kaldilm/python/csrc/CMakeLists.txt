include_directories(${openfst_SOURCE_DIR}/src/include)
pybind11_add_module(_kaldilm kaldilm.cc)
target_link_libraries(_kaldilm PRIVATE kaldilm_core)
