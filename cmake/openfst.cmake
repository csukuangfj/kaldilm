# Copyright (c)  2020  Xiaomi Corporation (author: Fangjun Kuang)

function(download_openfst)
  if(CMAKE_VERSION VERSION_LESS 3.11)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
  endif()

  include(FetchContent)

  set(openfst_URL  "https://github.com/kkm000/openfst/archive/win/1.7.2.1.tar.gz")
  set(openfst_HASH "SHA256=e04e1dabcecf3a687ace699ccb43a8a27da385777a56e69da6e103344cc66bca")

  set(HAVE_BIN OFF CACHE BOOL "" FORCE)
  set(HAVE_SCRIPT OFF CACHE BOOL "" FORCE)
  set(HAVE_COMPACT OFF CACHE BOOL "" FORCE)
  set(HAVE_COMPRESS OFF CACHE BOOL "" FORCE)
  set(HAVE_CONST OFF CACHE BOOL "" FORCE)
  set(HAVE_FAR OFF CACHE BOOL "" FORCE)
  set(HAVE_GRM OFF CACHE BOOL "" FORCE)
  set(HAVE_PDT OFF CACHE BOOL "" FORCE)
  set(HAVE_MPDT OFF CACHE BOOL "" FORCE)
  set(HAVE_LINEAR OFF CACHE BOOL "" FORCE)
  set(HAVE_LOOKAHEAD OFF CACHE BOOL "" FORCE)
  set(HAVE_NGRAM OFF CACHE BOOL "" FORCE)
  set(HAVE_PYTHON OFF CACHE BOOL "" FORCE)
  set(HAVE_SPECIAL OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(openfst
    URL               ${openfst_URL}
    URL_HASH          ${openfst_HASH}
    PATCH_COMMAND
      sed -i s/enable_testing\(\)//g "src/CMakeLists.txt" &&
      sed -i s/add_subdirectory\(test\)//g "src/CMakeLists.txt" &&
      sed -i s/add_subdirectory\(script\)//g "src/CMakeLists.txt" &&
      sed -i s/add_subdirectory\(extensions\)//g "src/CMakeLists.txt"
  )

  FetchContent_GetProperties(openfst)
  if(NOT openfst_POPULATED)
    message(STATUS "Downloading openfst")
    FetchContent_Populate(openfst)
  endif()
  message(STATUS "openfst is downloaded to ${openfst_SOURCE_DIR}")
  add_subdirectory(${openfst_SOURCE_DIR} ${openfst_BINARY_DIR} EXCLUDE_FROM_ALL)
  set(openfst_SOURCE_DIR ${openfst_SOURCE_DIR} PARENT_SCOPE)
endfunction()

download_openfst()
