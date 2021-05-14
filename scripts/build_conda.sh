#!/usr/bin/env bash
#
# Copyright (c)  2021  Xiaomi Corp.       (author: Fangjun Kuang)

set -e

cur_dir=$(cd $(dirname $BASH_SOURCE) && pwd)
kaldilm_dir=$(cd $cur_dir/.. && pwd)
cd $kaldilm_dir

export KALDILM_ROOT_DIR=$kaldilm_dir
echo "KALDILM_ROOT_DIR: $KALDILM_ROOT_DIR"

KALDILM_PYTHON_VERSION=$(python3 -c "import sys; print(sys.version[:3])")
echo "KALDILM_PYTHON_VERSION: $KALDILM_PYTHON_VERSION"

export KALDILM_PYTHON_VERSION

if [ -z $KALDILM_CONDA_TOKEN ]; then
  echo "Auto upload to anaconda.org is disabled since KALDILM_CONDA_TOKEN is not set"
  conda build --no-test --no-anaconda-upload -c conda-forge ./scripts/conda/kaldilm
else
  conda build --no-test -c conda-forge --token $KALDILM_CONDA_TOKEN ./scripts/conda/kaldilm
fi
