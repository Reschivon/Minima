# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /home/reschivon/clion-2021.2.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/reschivon/clion-2021.2.3/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/reschivon/Minima

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/reschivon/Minima/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Minima.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/Minima.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Minima.dir/flags.make

CMakeFiles/Minima.dir/main.cpp.o: CMakeFiles/Minima.dir/flags.make
CMakeFiles/Minima.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/reschivon/Minima/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Minima.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Minima.dir/main.cpp.o -c /home/reschivon/Minima/main.cpp

CMakeFiles/Minima.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Minima.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/reschivon/Minima/main.cpp > CMakeFiles/Minima.dir/main.cpp.i

CMakeFiles/Minima.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Minima.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/reschivon/Minima/main.cpp -o CMakeFiles/Minima.dir/main.cpp.s

CMakeFiles/Minima.dir/src/Print.cpp.o: CMakeFiles/Minima.dir/flags.make
CMakeFiles/Minima.dir/src/Print.cpp.o: ../src/Print.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/reschivon/Minima/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Minima.dir/src/Print.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Minima.dir/src/Print.cpp.o -c /home/reschivon/Minima/src/Print.cpp

CMakeFiles/Minima.dir/src/Print.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Minima.dir/src/Print.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/reschivon/Minima/src/Print.cpp > CMakeFiles/Minima.dir/src/Print.cpp.i

CMakeFiles/Minima.dir/src/Print.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Minima.dir/src/Print.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/reschivon/Minima/src/Print.cpp -o CMakeFiles/Minima.dir/src/Print.cpp.s

# Object files for target Minima
Minima_OBJECTS = \
"CMakeFiles/Minima.dir/main.cpp.o" \
"CMakeFiles/Minima.dir/src/Print.cpp.o"

# External object files for target Minima
Minima_EXTERNAL_OBJECTS =

Minima: CMakeFiles/Minima.dir/main.cpp.o
Minima: CMakeFiles/Minima.dir/src/Print.cpp.o
Minima: CMakeFiles/Minima.dir/build.make
Minima: /usr/lib/x86_64-linux-gnu/libcurses.so
Minima: CMakeFiles/Minima.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/reschivon/Minima/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable Minima"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Minima.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Minima.dir/build: Minima
.PHONY : CMakeFiles/Minima.dir/build

CMakeFiles/Minima.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Minima.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Minima.dir/clean

CMakeFiles/Minima.dir/depend:
	cd /home/reschivon/Minima/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/reschivon/Minima /home/reschivon/Minima /home/reschivon/Minima/cmake-build-debug /home/reschivon/Minima/cmake-build-debug /home/reschivon/Minima/cmake-build-debug/CMakeFiles/Minima.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Minima.dir/depend

