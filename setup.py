#!/usr/bin/env python3
#
# Copyright (c)  2020  Xiaomi Corporation (author: Fangjun Kuang)

import glob
import os
import re
import setuptools
import shutil
import subprocess

from setuptools.command.build_ext import build_ext


def cmake_extension(name, *args, **kwargs) -> setuptools.Extension:
    kwargs['language'] = 'c++'
    sources = []
    return setuptools.Extension(name, sources, *args, **kwargs)


class BuildExtension(build_ext):
    def build_extension(self, ext: setuptools.extension.Extension):
        build_dir = 'build'
        os.makedirs(build_dir, exist_ok=True)

        os.makedirs(self.build_lib, exist_ok=True)

        subprocess.check_call(['cmake', '--S', '.', '-B', f'{build_dir}'])
        subprocess.check_call(
            ['cmake', '--build', f'{build_dir}', '--target', '_kaldilm'])
        lib_so = glob.glob(f'{build_dir}/lib/*.so')
        for so in lib_so:
            shutil.copy(f'{so}', f'{self.build_lib}/')


def get_package_version():
    with open('CMakeLists.txt') as f:
        content = f.read()

    latest_version = re.search(r'set\(kaldilm_VERSION (.*)\)',
                               content).group(1)
    latest_version = latest_version.strip('"')
    return latest_version


package_name = 'kaldilm'

setuptools.setup(
    name=package_name,
    version=get_package_version(),
    author='Fangjun Kuang',
    author_email='csukuangfj@gmail.com',
    data_files=[('', ['LICENSE', 'README.md'])],
    package_dir={
        package_name: 'kaldilm/python/kaldilm',
    },
    packages=[package_name],
    url='https://github.com/csukuangfj/kaldilm',
    ext_modules=[cmake_extension(package_name)],
    cmdclass={"build_ext": BuildExtension},
    zip_safe=False,
)
