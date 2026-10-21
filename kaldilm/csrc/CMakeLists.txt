
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${openfst_SOURCE_DIR}/src/include)

set(kaldilm_srcs
  arpa_file_parser.cc
  arpa_lm_compiler.cc
  string_utils.cc
)

# Explicitly static, like kaldifst_core: the _kaldilm extension then links the
# code in and becomes self-contained, so the wheel needs no extra libraries and
# no RPATH fixups.
add_library(kaldilm_core STATIC ${kaldilm_srcs})

if(NOT WIN32)
  set_target_properties(kaldilm_core
    PROPERTIES
      POSITION_INDEPENDENT_CODE ON
      C_VISIBILITY_PRESET hidden
      CXX_VISIBILITY_PRESET hidden
  )
endif()

target_link_libraries(kaldilm_core PUBLIC fst)

if(APPLE)
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGE_DIR
  )
  message(STATUS "PYTHON_SITE_PACKAGE_DIR: ${PYTHON_SITE_PACKAGE_DIR}")
  target_link_libraries(kaldilm_core PUBLIC "-L ${PYTHON_SITE_PACKAGE_DIR}/../..")
endif()

if(KALDILM_BUILD_TESTS)
  add_executable(arpa_file_parser_test arpa_file_parser_test.cc)
  target_link_libraries(arpa_file_parser_test kaldilm_core)
  target_compile_definitions(arpa_file_parser_test PRIVATE KALDILM_TEST_DATA_DIR=${CMAKE_CURRENT_LIST_DIR})

  add_executable(arpa_lm_compiler_test arpa_lm_compiler_test.cc)
  target_link_libraries(arpa_lm_compiler_test kaldilm_core)
  target_compile_definitions(arpa_lm_compiler_test PRIVATE KALDILM_TEST_DATA_DIR=${CMAKE_CURRENT_LIST_DIR})

  add_test(NAME arpa_file_parser_test COMMAND arpa_file_parser_test)
  add_test(NAME arpa_lm_compiler_test COMMAND arpa_lm_compiler_test)
endif()
