from distutils.core import setup, Extension


includeDirs = ['/usr/local/include', './include']

libs = ['pqxx', 'pq']

src = [ './src/routing/dijkstra.cpp', './src/routing/edge.cpp', './src/routing/routing_module.cpp', './src/routing/vertex.cpp', './src/database/database_helper.cpp']

module = Extension('routing_module',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = includeDirs,
                    libraries = libs,
                    library_dirs = ['/usr/local/lib'],
                    sources = src)

setup (name = 'PackageName',
       version = '1.0',
       description = 'Package for routing.',
       ext_modules = [module])

