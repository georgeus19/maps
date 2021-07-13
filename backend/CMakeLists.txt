cmake_minimum_required(VERSION 3.16)
project(backend)

set(CMAKE_CXX_STANDARD 20)
# set (CMAKE_CXX_FLAGS "-std=c++17 -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -DMAJOR_VERSION=1 -DMINOR_VERSION=0")
set(CMAKE_CXX_FLAGS "-Wall")
file(GLOB OsmGraphBuilderGlob src/graph_builder/*.cpp include/graph_builder/*.h)

file(GLOB DatabaseGlob src/routing/database/*.cpp include/routing/database/*.h)

file(GLOB UtilityGlob src/routing/utility/*.cpp include/routing/utility/*.h)

file(GLOB RoutingGlob src/routing/*.cpp include/routing/*.h)
file(GLOB RoutingVertexGlob src/routing/vertices/*.cpp include/routing/vertices/*.h src/routing/edge_ranges/*.cpp include/routing/edge_ranges/*.h)
file(GLOB RoutingEdgeGlob src/routing/edges/*.cpp include/routing/edges/*.h)
file(GLOB RoutingQueryGlob src/routing/query/*.cpp include/routing/query/*.h)
file(GLOB RoutingPreprocessingGlob src/routing/preprocessing/*.cpp include/routing/preprocessing/*.h )
file(GLOB RoutingProfileGlob src/routing/profile/*.cpp include/routing/profile/*.h)

file(GLOB OthersGlob src/*.cpp include/*.h)

file(GLOB RoutingTestsGlob src/tests/*.cpp include/tests/*.h)

file(GLOB RoutingServerGlob src/routing/server/*.cpp include/routing/server/*.h)

file(GLOB RoutingPreprocessorGlob src/routing/preprocessor/*.cpp include/routing/preprocessor/*.h)


# libraries
add_library(routing ${RoutingGlob} ${RoutingVertexGlob} ${RoutingEdgeGlob} ${RoutingQueryGlob} ${RoutingPreprocessingGlob} ${RoutingProfileGlob}
                    ${DatabaseGlob} ${UtilityGlob})

# tools
add_executable(graph_builder ${OsmGraphBuilderGlob})
add_executable(routing_preprocessor ${RoutingPreprocessorGlob})
add_executable(routing_server ${RoutingServerGlob})

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

include_directories(include/)

target_link_libraries(graph_builder -lpthread -lz -lexpat -lbz2)

include_directories(/usr/include/python3.8/)
include_directories(lib)

include_directories(/usr/local/include)
include_directories(/usr/local/include/mapnik/agg /usr/local/include/mapnik)
include_directories(/usr/local/include /usr/include /usr/include/freetype2 /usr/include/postgresql /usr/include/cairo /usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/include/pixman-1 /usr/include/uuid /usr/include/libpng16)

target_link_libraries(tests routing)

target_link_libraries(routing_server routing)
target_link_libraries(routing_server pthread boost_system z)

target_link_libraries(routing pqxx)
target_link_libraries(routing pq)

target_link_libraries(routing_preprocessor routing)

SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2")
# SET(CMAKE_CXX_FLAGS_BUILD "-g -pg -O2")
# set (CMAKE_CXX_FLAGS "-std=c++17 -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -DMAJOR_VERSION=1 -DMINOR_VERSION=0")