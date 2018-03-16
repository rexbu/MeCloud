#! /usr/bin/env python
# -*- coding: utf-8 -*-
import os
from distutils.command.build_py import build_py as _build_py
from distutils.core import setup, Extension


#class build_py(_build_py):
#    def run(self):
#        os.system('cp -rf server/mecloud.py /usr/local/bin/')


import distutils.core



#打包，解压，过程。
#打包：在MeCloud下面执行，python setup sdist，就会根据下面的setup进行打包的操作，会再Mecloud下面生成一个dist文件夹，里面有一个war包，就是打包好的文件包了。
#解压安装：把打好的包，进行解压，解压后的文件夹中有一个setup.py，执行python setup.py install就可以对解压后的包进行安装了。






version = "1.0"

distutils.core.setup(
    name="mecloud",
    version=version,
    author="blinnnk",
    package_dir={'mecloud': 'server'},
    packages=["mecloud", "mecloud.api", "mecloud.helper", "mecloud.lib", "mecloud.model", "mecloud.constants"],
    ext_modules=[Extension('mecloud.lib.crypto_string',sources = ['server/lib/crypto_string.cpp'],library_dirs = ['server/lib'])]
    #cmdclass={'build_py': build_py}
)
