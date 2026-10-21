#!/usr/bin/env python3
#
# Copyright (c)  2026  Xiaomi Corporation (author: Fangjun Kuang)
#
# Smoke test for the built wheels. Run after installing a freshly built wheel.
#
# It compiles a real ARPA language model so that a wheel that cannot load its
# extension module (missing shared library, wrong architecture, wrong platform
# tag) fails here rather than on a user's machine.
#
# It uses no third-party dependencies -- only the kaldilm package itself -- so
# it runs anywhere the wheel installs.

import os
import platform
import sys
import tempfile

import kaldilm


def scratch_parent():
    """Return a writable directory for scratch files.

    Termux has no /tmp and its / is read-only, so the usual temp locations are
    not available there. Try them in order and fall back to the cwd.
    """
    for candidate in (os.environ.get("TMPDIR"), "/tmp", "/var/tmp", os.getcwd()):
        if candidate and os.path.isdir(candidate) and os.access(candidate, os.W_OK):
            return candidate
    raise RuntimeError("no writable scratch directory found")

# A minimal but complete ARPA model. It must contain both the begin- and
# end-of-sentence symbols (<s> and </s>), otherwise ArpaLmCompiler::Check()
# rejects it. Fields are tab-separated and sections are marked with
# backslash-delimited keywords -- the doubled backslashes below are the Python
# escapes for those literal backslashes, while \t is a real tab.
ARPA = """
\\data\\
ngram 1=3
ngram 2=2

\\1-grams:
-1.0000000\t<s>\t-0.5000000
-1.0000000\t</s>
-1.0000000\ta\t-0.5000000

\\2-grams:
-0.5000000\t<s>\ta
-0.5000000\ta\t</s>

\\end\\
"""


def main() -> int:
    print("kaldilm:", getattr(kaldilm, "__version__", "<none>"), kaldilm.__file__)
    print("python:", sys.version.replace("\n", " "))
    print("platform:", platform.platform())
    print("machine:", platform.machine())

    with tempfile.TemporaryDirectory(dir=scratch_parent()) as tmp:
        arpa_path = os.path.join(tmp, "input.arpa")
        fst_path = os.path.join(tmp, "out.fst")
        with open(arpa_path, "w", encoding="utf-8") as f:
            f.write(ARPA)

        text = kaldilm.arpa2fst(arpa_path, output_fst=fst_path)
        print("arpa2fst returned:", type(text).__name__, len(text), "chars")

        assert os.path.isfile(fst_path), "no FST was written"
        size = os.path.getsize(fst_path)
        print("out.fst size:", size)
        assert size > 0, "the FST is empty"

    print("SMOKE TEST PASSED")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
