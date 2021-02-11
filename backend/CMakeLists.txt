cmake_minimum_required(VERSION 3.16)
project(backend)

set(CMAKE_CXX_STANDARD 20)
file(GLOB OsmParserGlob src/osm_parser/*.cpp include/osm_parser/*.h)
file(GLOB DatabaseHelperGlob src/database/*.cpp include/database/*.h)
file(GLOB TileRenderingGlob src/tile_rendering/*.cpp include/tile_rendering/*.h)
file(GLOB OthersGlob src/*.cpp include/*.h)
file(GLOB RoutingGLob src/routing/*.cpp include/routing/*.h include/src/routing/vertices/*.h)
file(GLOB UtilityGlob src/utility/*.cpp include/utility/*.h)
file(GLOB RoutingPreprocessingGlob src/routing/preprocessing/*.cpp include/routing/preprocessing/*.h)
file(GLOB RoutingTestsGlob src/tests/*.cpp include/tests/*.h)




add_executable(backend_test ${OthersGlob} ${DatabaseHelperGlob} ${UtilityGlob})
add_library(render ${TileRenderingGlob})
add_executable(graph_builder ${OsmParserGlob})
add_library(routing ${RoutingGLob} ${RoutingPreprocessingGlob} ${DatabaseHelperGlob} ${UtilityGlob})
# add_executable(routing_preprocessing ${RoutingGLob} ${RoutingPreprocessingGlob} ${DatabaseHelperGlob} ${UtilityGlob})
add_library(routing_preprocessing ${RoutingGLob} ${RoutingPreprocessingGlob} ${DatabaseHelperGlob} ${UtilityGlob})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Osmium REQUIRED)
if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()
include_directories(${OSMIUM_INCLUDE_DIRS})



# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

include (CTest)
# Now simply link against gtest or gtest_main as needed. Eg
add_executable(tests ${RoutingTestsGlob})
include_directories(include/)
link_directories(tests build/)
target_link_libraries(tests gtest_main gmock_main)
add_test(NAME dijkstra_test COMMAND dijkstra_test)

include_directories(src/osm_parser/)
include_directories(src/tile_rendering/)
include_directories(src/database_helper/)
include_directories(src/routing/)
include_directories(src/utility/)
include_directories(src/routing/preprocessing)

include_directories(include/)
include_directories(include/osm_parser/)
include_directories(include/tile_rendering/)
include_directories(include/database_helper/)
include_directories(include/routing/)
include_directories(include/routing/vertices/)
include_directories(include/utility/)
include_directories(include/routing/preprocessing)

target_link_libraries(graph_builder -lpthread -lz -lexpat -lbz2)

include_directories(/usr/include/python3.8/)

link_directories(render /usr/local/li)

include_directories(/usr/local/include)
include_directories(/usr/local/include/mapnik/agg /usr/local/include/mapnik)
include_directories(/usr/local/include /usr/include /usr/include/freetype2 /usr/include/postgresql /usr/include/cairo /usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/include/pixman-1 /usr/include/uuid /usr/include/libpng16)

link_directories(backend_test /usr/local/lib)

target_link_libraries(tests routing)
target_link_libraries(tests routing_preprocessing)

target_link_libraries(routing pqxx)
target_link_libraries(routing pq)

target_link_libraries(routing_preprocessing pqxx)
target_link_libraries(routing_preprocessing pq)

target_link_libraries(backend_test mapnik boost_filesystem boost_regex cairo png proj tiff webp icui18n boost_system harfbuzz jpeg icuuc freetype z dl)
target_link_libraries(backend_test libmapnik-json.a libmapnik-wkt.a )
target_link_libraries(backend_test pqxx)
target_link_libraries(backend_test pq)

