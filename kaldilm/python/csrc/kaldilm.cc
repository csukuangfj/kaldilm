// kaldilm/python/csrc/kaldilm.cc
//
// Copyright (c)  2020  Xiaomi Corporation (authors: Daniel Povey)

#include "kaldilm/python/csrc/kaldilm.h"

PYBIND11_MODULE(kaldilm, m) { m.doc() = "Python wrapper for kaldilm"; }
