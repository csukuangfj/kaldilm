include_directories(${CMAKE_SOURCE_DIR})
include_directories(${openfst_SOURCE_DIR}/src/include)

pybind11_add_module(_kaldilm kaldilm.cc)
target_link_libraries(_kaldilm PRIVATE kaldilm_core)

# Install into a lib/ sub-directory of the package, like kaldifst does:
# kaldilm/lib/_kaldilm.<ext>. setup.py runs `cmake --install` with
# CMAKE_INSTALL_PREFIX pointing at build_lib/kaldilm, so this rule is what
# places the extension in the wheel.
install(TARGETS _kaldilm DESTINATION lib)

if(APPLE)
  execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGE_DIR
  )
  message(STATUS "PYTHON_SITE_PACKAGE_DIR: ${PYTHON_SITE_PACKAGE_DIR}")
  target_link_libraries(_kaldilm PRIVATE "-Wl,-rpath,${PYTHON_SITE_PACKAGE_DIR}")
endif()
