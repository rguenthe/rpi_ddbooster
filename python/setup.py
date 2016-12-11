#!/usr/bin/env python

# python wrapper for ddbooster library
# Author: Richard Guenther

from distutils.core import setup, Extension

setup(name              = 'ddbooster',
      version           = '1.0.0',
      author            = 'Richard Guenther',
      author_email      = 'richard.guenther@web.de',
      description       = 'Raspberry Pi userspace library for DIGI-DOT-BOOSTER.',
      license           = 'MIT',
      py_modules        = ["ddbooster"],
      ext_modules       = [Extension('_ddbooster',
                                     sources=['ddbooster.i', '../DDBooster.cpp'],
                                     libraries=['wiringPi'],
                                     swig_opts=['-c++'],
                                    )]
)