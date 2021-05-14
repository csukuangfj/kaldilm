#!/usr/bin/env bash
#
# Copyright (c)  2021  Xiaomi Corp.       (author: Fangjun Kuang)

set -ex

echo "KALDILM_PYTHON_VERSION: $KALDILM_PYTHON_VERSION"

python3 setup.py install --single-version-externally-managed --record=record.txt

