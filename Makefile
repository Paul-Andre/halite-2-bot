# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/paul/Programming/Projects/Halite

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/paul/Programming/Projects/Halite

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/paul/Programming/Projects/Halite/CMakeFiles /home/paul/Programming/Projects/Halite/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/paul/Programming/Projects/Halite/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named MyBot

# Build rule for target.
MyBot: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 MyBot
.PHONY : MyBot

# fast build rule for target.
MyBot/fast:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/build
.PHONY : MyBot/fast

MyBot.o: MyBot.cpp.o

.PHONY : MyBot.o

# target to build an object file
MyBot.cpp.o:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/MyBot.cpp.o
.PHONY : MyBot.cpp.o

MyBot.i: MyBot.cpp.i

.PHONY : MyBot.i

# target to preprocess a source file
MyBot.cpp.i:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/MyBot.cpp.i
.PHONY : MyBot.cpp.i

MyBot.s: MyBot.cpp.s

.PHONY : MyBot.s

# target to generate assembly for a file
MyBot.cpp.s:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/MyBot.cpp.s
.PHONY : MyBot.cpp.s

hlt/hlt_in.o: hlt/hlt_in.cpp.o

.PHONY : hlt/hlt_in.o

# target to build an object file
hlt/hlt_in.cpp.o:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/hlt_in.cpp.o
.PHONY : hlt/hlt_in.cpp.o

hlt/hlt_in.i: hlt/hlt_in.cpp.i

.PHONY : hlt/hlt_in.i

# target to preprocess a source file
hlt/hlt_in.cpp.i:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/hlt_in.cpp.i
.PHONY : hlt/hlt_in.cpp.i

hlt/hlt_in.s: hlt/hlt_in.cpp.s

.PHONY : hlt/hlt_in.s

# target to generate assembly for a file
hlt/hlt_in.cpp.s:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/hlt_in.cpp.s
.PHONY : hlt/hlt_in.cpp.s

hlt/location.o: hlt/location.cpp.o

.PHONY : hlt/location.o

# target to build an object file
hlt/location.cpp.o:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/location.cpp.o
.PHONY : hlt/location.cpp.o

hlt/location.i: hlt/location.cpp.i

.PHONY : hlt/location.i

# target to preprocess a source file
hlt/location.cpp.i:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/location.cpp.i
.PHONY : hlt/location.cpp.i

hlt/location.s: hlt/location.cpp.s

.PHONY : hlt/location.s

# target to generate assembly for a file
hlt/location.cpp.s:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/location.cpp.s
.PHONY : hlt/location.cpp.s

hlt/map.o: hlt/map.cpp.o

.PHONY : hlt/map.o

# target to build an object file
hlt/map.cpp.o:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/map.cpp.o
.PHONY : hlt/map.cpp.o

hlt/map.i: hlt/map.cpp.i

.PHONY : hlt/map.i

# target to preprocess a source file
hlt/map.cpp.i:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/map.cpp.i
.PHONY : hlt/map.cpp.i

hlt/map.s: hlt/map.cpp.s

.PHONY : hlt/map.s

# target to generate assembly for a file
hlt/map.cpp.s:
	$(MAKE) -f CMakeFiles/MyBot.dir/build.make CMakeFiles/MyBot.dir/hlt/map.cpp.s
.PHONY : hlt/map.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... MyBot"
	@echo "... MyBot.o"
	@echo "... MyBot.i"
	@echo "... MyBot.s"
	@echo "... hlt/hlt_in.o"
	@echo "... hlt/hlt_in.i"
	@echo "... hlt/hlt_in.s"
	@echo "... hlt/location.o"
	@echo "... hlt/location.i"
	@echo "... hlt/location.s"
	@echo "... hlt/map.o"
	@echo "... hlt/map.i"
	@echo "... hlt/map.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

