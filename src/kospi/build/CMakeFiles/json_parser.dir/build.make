# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/soms/Honours/src/kospi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/soms/Honours/src/kospi/build

# Include any dependencies generated for this target.
include CMakeFiles/json_parser.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/json_parser.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/json_parser.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/json_parser.dir/flags.make

CMakeFiles/json_parser.dir/main.cpp.o: CMakeFiles/json_parser.dir/flags.make
CMakeFiles/json_parser.dir/main.cpp.o: ../main.cpp
CMakeFiles/json_parser.dir/main.cpp.o: CMakeFiles/json_parser.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/soms/Honours/src/kospi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/json_parser.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/json_parser.dir/main.cpp.o -MF CMakeFiles/json_parser.dir/main.cpp.o.d -o CMakeFiles/json_parser.dir/main.cpp.o -c /home/soms/Honours/src/kospi/main.cpp

CMakeFiles/json_parser.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/json_parser.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/soms/Honours/src/kospi/main.cpp > CMakeFiles/json_parser.dir/main.cpp.i

CMakeFiles/json_parser.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/json_parser.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/soms/Honours/src/kospi/main.cpp -o CMakeFiles/json_parser.dir/main.cpp.s

# Object files for target json_parser
json_parser_OBJECTS = \
"CMakeFiles/json_parser.dir/main.cpp.o"

# External object files for target json_parser
json_parser_EXTERNAL_OBJECTS =

json_parser: CMakeFiles/json_parser.dir/main.cpp.o
json_parser: CMakeFiles/json_parser.dir/build.make
json_parser: CMakeFiles/json_parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/soms/Honours/src/kospi/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable json_parser"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/json_parser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/json_parser.dir/build: json_parser
.PHONY : CMakeFiles/json_parser.dir/build

CMakeFiles/json_parser.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/json_parser.dir/cmake_clean.cmake
.PHONY : CMakeFiles/json_parser.dir/clean

CMakeFiles/json_parser.dir/depend:
	cd /home/soms/Honours/src/kospi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/soms/Honours/src/kospi /home/soms/Honours/src/kospi /home/soms/Honours/src/kospi/build /home/soms/Honours/src/kospi/build /home/soms/Honours/src/kospi/build/CMakeFiles/json_parser.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/json_parser.dir/depend

