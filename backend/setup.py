import os
from distutils.core import setup, Extension

os.environ['CC'] = 'g++-9 -std=c++2a'
includeDirs = ['/usr/local/include', './include', './lib']

libs = ['pqxx', 'pq']

src = ['./src/routing/exception.cpp', './src/routing/query/module.cpp', './src/routing/edges/ch_search_edge.cpp', './src/routing/edges/basic_edge.cpp', './src/routing/edges/ch_preprocessing_edge.cpp', './src/routing/preprocessing/routing_preprocessor.cpp', './src/database/database_helper.cpp', './src/utility/point.cpp']

module = Extension('routing_module',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = includeDirs,
                    libraries = libs,
                    library_dirs = ['/usr/local/lib'],
                    sources = src)

setup (name = 'MapRouting',
       version = '1.0',
       description = 'Package for routing.',
       ext_modules = [module])

