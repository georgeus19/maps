# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /snap/clion/124/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/124/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hrubyk/projects/maps/backend

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hrubyk/projects/maps/backend/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/graph_builder.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/graph_builder.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/graph_builder.dir/flags.make

CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.o: CMakeFiles/graph_builder.dir/flags.make
CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.o: ../src/osm_parser/edge.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hrubyk/projects/maps/backend/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.o -c /home/hrubyk/projects/maps/backend/src/osm_parser/edge.cpp

CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hrubyk/projects/maps/backend/src/osm_parser/edge.cpp > CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.i

CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hrubyk/projects/maps/backend/src/osm_parser/edge.cpp -o CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.s

CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.o: CMakeFiles/graph_builder.dir/flags.make
CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.o: ../src/osm_parser/link_counter.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hrubyk/projects/maps/backend/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.o -c /home/hrubyk/projects/maps/backend/src/osm_parser/link_counter.cpp

CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hrubyk/projects/maps/backend/src/osm_parser/link_counter.cpp > CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.i

CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hrubyk/projects/maps/backend/src/osm_parser/link_counter.cpp -o CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.s

CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.o: CMakeFiles/graph_builder.dir/flags.make
CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.o: ../src/osm_parser/osm_parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hrubyk/projects/maps/backend/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.o -c /home/hrubyk/projects/maps/backend/src/osm_parser/osm_parser.cpp

CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hrubyk/projects/maps/backend/src/osm_parser/osm_parser.cpp > CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.i

CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hrubyk/projects/maps/backend/src/osm_parser/osm_parser.cpp -o CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.s

CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.o: CMakeFiles/graph_builder.dir/flags.make
CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.o: ../src/osm_parser/writer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hrubyk/projects/maps/backend/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.o -c /home/hrubyk/projects/maps/backend/src/osm_parser/writer.cpp

CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hrubyk/projects/maps/backend/src/osm_parser/writer.cpp > CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.i

CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hrubyk/projects/maps/backend/src/osm_parser/writer.cpp -o CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.s

# Object files for target graph_builder
graph_builder_OBJECTS = \
"CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.o" \
"CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.o" \
"CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.o" \
"CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.o"

# External object files for target graph_builder
graph_builder_EXTERNAL_OBJECTS =

graph_builder: CMakeFiles/graph_builder.dir/src/osm_parser/edge.cpp.o
graph_builder: CMakeFiles/graph_builder.dir/src/osm_parser/link_counter.cpp.o
graph_builder: CMakeFiles/graph_builder.dir/src/osm_parser/osm_parser.cpp.o
graph_builder: CMakeFiles/graph_builder.dir/src/osm_parser/writer.cpp.o
graph_builder: CMakeFiles/graph_builder.dir/build.make
graph_builder: CMakeFiles/graph_builder.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hrubyk/projects/maps/backend/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable graph_builder"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/graph_builder.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/graph_builder.dir/build: graph_builder

.PHONY : CMakeFiles/graph_builder.dir/build

CMakeFiles/graph_builder.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/graph_builder.dir/cmake_clean.cmake
.PHONY : CMakeFiles/graph_builder.dir/clean

CMakeFiles/graph_builder.dir/depend:
	cd /home/hrubyk/projects/maps/backend/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hrubyk/projects/maps/backend /home/hrubyk/projects/maps/backend /home/hrubyk/projects/maps/backend/cmake-build-debug /home/hrubyk/projects/maps/backend/cmake-build-debug /home/hrubyk/projects/maps/backend/cmake-build-debug/CMakeFiles/graph_builder.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/graph_builder.dir/depend

