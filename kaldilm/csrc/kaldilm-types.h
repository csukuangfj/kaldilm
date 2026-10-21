// kaldilm/csrc/kaldilm-types.h
//
// Copyright (c)  2026  Xiaomi Corporation (author: Fangjun Kuang)

#ifndef KALDILM_CSRC_KALDILM_TYPES_H_
#define KALDILM_CSRC_KALDILM_TYPES_H_

// Fixed-width aliases in the Kaldi style.
//
// These used to come transitively from openfst's compat.h. openfst 1.8.x no
// longer provides them, so define them here rather than sprinkling <cstdint>
// spellings through the Kaldi-derived sources.

#include <cstdint>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

#endif  // KALDILM_CSRC_KALDILM_TYPES_H_
