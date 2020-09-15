cmake_minimum_required(VERSION 3.17)
project(backend)

set(CMAKE_CXX_STANDARD 20)
file(GLOB OsmParserGlob src/osm_parser/*.cpp include/osm_parser/*.h)
file(GLOB DatabaseHelperGlob src/database/*.cpp include/database/*.h)
file(GLOB TileRenderingGlob src/tile_rendering/*.cpp include/tile_rendering/*.h)
file(GLOB OthersGlob src/*.cpp include/*.h)
file(GLOB RoutingGLob src/routing/*.cpp include/routing/*.h)
file(GLOB UtilityGlob src/utility/*.cpp include/utility/*.h)

add_library(backend ${OthersGlob} ${DatabaseHelperGlob} ${UtilityGlob})
add_executable(backend_test ${OthersGlob} ${DatabaseHelperGlob} ${UtilityGlob})
add_library(render ${TileRenderingGlob})
add_executable(graph_builder ${OsmParserGlob})
add_library(routing ${RoutingGLob} ${DatabaseHelperGlob} ${UtilityGlob})

include_directories(src/osm_parser/)
include_directories(src/tile_rendering/)
include_directories(src/database_helper/)
include_directories(src/routing/)
include_directories(src/utility/)

include_directories(include/)
include_directories(include/osm_parser/)
include_directories(include/tile_rendering/)
include_directories(include/database_helper/)
include_directories(include/routing/)
include_directories(include/utility/)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Osmium REQUIRED)
if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()
include_directories(${OSMIUM_INCLUDE_DIRS})

target_link_libraries(graph_builder -lpthread -lz -lexpat -lbz2)

include_directories(/usr/include/python3.8/)

link_directories(render /usr/local/li)

include_directories(/usr/local/include)
include_directories(/usr/local/include/mapnik/agg /usr/local/include/mapnik)
include_directories(/usr/local/include /usr/include /usr/include/freetype2 /usr/include/postgresql /usr/include/cairo /usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/include/pixman-1 /usr/include/uuid /usr/include/libpng16)

link_directories(backend_test /usr/local/lib)

target_link_libraries(routing pqxx)
target_link_libraries(routing pq)

target_link_libraries(backend pqxx)
target_link_libraries(backend pq)

target_link_libraries(backend_test mapnik boost_filesystem boost_regex cairo png proj tiff webp icui18n boost_system harfbuzz jpeg icuuc freetype z dl)
target_link_libraries(backend_test libmapnik-json.a libmapnik-wkt.a )
target_link_libraries(backend_test pqxx)
target_link_libraries(backend_test pq)


