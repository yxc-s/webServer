# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yxc/文档/server/day22

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yxc/文档/server/day22/build

# Include any dependencies generated for this target.
include CMakeFiles/hpts.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/hpts.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hpts.dir/flags.make

CMakeFiles/hpts.dir/hpts.cpp.o: CMakeFiles/hpts.dir/flags.make
CMakeFiles/hpts.dir/hpts.cpp.o: ../hpts.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yxc/文档/server/day22/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/hpts.dir/hpts.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hpts.dir/hpts.cpp.o -c /home/yxc/文档/server/day22/hpts.cpp

CMakeFiles/hpts.dir/hpts.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hpts.dir/hpts.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yxc/文档/server/day22/hpts.cpp > CMakeFiles/hpts.dir/hpts.cpp.i

CMakeFiles/hpts.dir/hpts.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hpts.dir/hpts.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yxc/文档/server/day22/hpts.cpp -o CMakeFiles/hpts.dir/hpts.cpp.s

CMakeFiles/hpts.dir/hpts.cpp.o.requires:

.PHONY : CMakeFiles/hpts.dir/hpts.cpp.o.requires

CMakeFiles/hpts.dir/hpts.cpp.o.provides: CMakeFiles/hpts.dir/hpts.cpp.o.requires
	$(MAKE) -f CMakeFiles/hpts.dir/build.make CMakeFiles/hpts.dir/hpts.cpp.o.provides.build
.PHONY : CMakeFiles/hpts.dir/hpts.cpp.o.provides

CMakeFiles/hpts.dir/hpts.cpp.o.provides.build: CMakeFiles/hpts.dir/hpts.cpp.o


# Object files for target hpts
hpts_OBJECTS = \
"CMakeFiles/hpts.dir/hpts.cpp.o"

# External object files for target hpts
hpts_EXTERNAL_OBJECTS =

bin/hpts: CMakeFiles/hpts.dir/hpts.cpp.o
bin/hpts: CMakeFiles/hpts.dir/build.make
bin/hpts: lib/libsockbase_lib.so
bin/hpts: lib/libhttpbase_lib.so
bin/hpts: CMakeFiles/hpts.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yxc/文档/server/day22/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/hpts"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hpts.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hpts.dir/build: bin/hpts

.PHONY : CMakeFiles/hpts.dir/build

CMakeFiles/hpts.dir/requires: CMakeFiles/hpts.dir/hpts.cpp.o.requires

.PHONY : CMakeFiles/hpts.dir/requires

CMakeFiles/hpts.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hpts.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hpts.dir/clean

CMakeFiles/hpts.dir/depend:
	cd /home/yxc/文档/server/day22/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yxc/文档/server/day22 /home/yxc/文档/server/day22 /home/yxc/文档/server/day22/build /home/yxc/文档/server/day22/build /home/yxc/文档/server/day22/build/CMakeFiles/hpts.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hpts.dir/depend

