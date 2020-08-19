cmake_minimum_required(VERSION 3.17)
project(backend)

set(CMAKE_CXX_STANDARD 20)

add_library(backend library.cpp library.h DatabaseHelper.cpp DatabaseHelper.h)
add_executable(backend_test test.cpp DatabaseHelper.cpp DatabaseHelper.h)


include_directories(/usr/local/include)
include_directories(/usr/local/include/mapnik/agg /usr/local/include/mapnik)
include_directories(/usr/local/include /usr/include /usr/include/freetype2 /usr/include/postgresql /usr/include/cairo /usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/include/pixman-1 /usr/include/uuid /usr/include/libpng16)

link_directories(backend_test /usr/local/lib)


target_link_libraries(backend pqxx)
target_link_libraries(backend pq)

target_link_libraries(backend_test mapnik boost_filesystem boost_regex cairo png proj tiff webp icui18n boost_system harfbuzz jpeg icuuc freetype z dl)
target_link_libraries(backend_test libmapnik-json.a libmapnik-wkt.a )
target_link_libraries(backend_test pqxx)
target_link_libraries(backend_test pq)


