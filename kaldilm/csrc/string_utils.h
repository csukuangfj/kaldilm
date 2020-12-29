// kaldilm/csrc/string_utils.h
//
// Copyright (c)  2020  Xiaomi Corporation (authors: Fangjun Kuang)

#ifndef KALDILM_CSRC_STRING_UTILS_H_
#define KALDILM_CSRC_STRING_UTILS_H_

#include <string>
#include <vector>

namespace kaldilm {

void SplitString(char *s, const char *delim, bool omit_empty_strings,
                 std::vector<char *> *vec);

void SplitString(const std::string &s, const char *delim,
                 bool omit_empty_strings, std::vector<std::string> *vec);

bool ConvertStringToInteger(const std::string &s, int32_t *out);
bool ConvertStringToReal(const std::string &s, float *out);

}  // namespace kaldilm

#endif  // KALDILM_CSRC_STRING_UTILS_H_
