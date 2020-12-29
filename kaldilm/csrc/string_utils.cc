// kaldilm/csrc/string_utils.cc
//
// Copyright (c)  2020  Xiaomi Corporation (authors: Fangjun Kuang)

#include "kaldilm/csrc/string_utils.h"

#include <string.h>

#include <sstream>
#include <string>

namespace kaldilm {
// copied from OpenFST
void SplitString(char *full, const char *delim, bool omit_empty_strings,
                 std::vector<char *> *vec) {
  vec->clear();

  char *p = full;
  while (p) {
    if ((p = strpbrk(full, delim))) {
      p[0] = '\0';
    }
    if (!omit_empty_strings || full[0] != '\0') vec->push_back(full);
    if (p) full = p + 1;
  }
}

void SplitString(const std::string &s, const char *delim,
                 bool omit_empty_strings, std::vector<std::string> *vec) {
  vec->clear();
  std::string saved = s;

  char *full = &saved[0];
  char *p = full;
  while (p) {
    if ((p = strpbrk(full, delim))) {
      p[0] = '\0';
    }
    if (!omit_empty_strings || full[0] != '\0') vec->push_back(full);
    if (p) full = p + 1;
  }
}

bool ConvertStringToInteger(const std::string &s, int32_t *out) {
  std::stringstream ss;
  ss << s;
  ss >> *out;
  return (bool)ss;
}

bool ConvertStringToReal(const std::string &s, float *out) {
  std::stringstream ss;
  ss << s;
  ss >> *out;
  return (bool)ss;
}

}  // namespace kaldilm
