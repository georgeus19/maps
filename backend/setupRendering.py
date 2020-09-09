from distutils.core import setup, Extension


includeDirs = ['/usr/local/include', '/usr/local/include/mapnik/agg', '/usr/local/include/mapnik', '/usr/local/include', '/usr/include', '/usr/include/freetype2', '/usr/include/libpng16', '/usr/include/postgresql', '/usr/include/gdal', '/usr/include/cairo', '/usr/include/glib-2.0', '/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/pixman-1', '/usr/include/uuid']

libs = ['mapnik', 'boost_filesystem', 'boost_regex', 'cairo', 'png', 'tiff', 'webp', 'icui18n', 'boost_system', 'harfbuzz', 'jpeg', 'icuuc', 'freetype', 'z', 'dl']

 

module = Extension('renderModule',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = includeDirs,
                    libraries = libs,
                    library_dirs = ['/usr/local/lib'],
                    sources = ['rendermodule.cpp'])

setup (name = 'PackageName',
       version = '1.0',
       description = 'Package for rendering tiles.',
       ext_modules = [module])

