# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aibot/wheelchair_bed/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aibot/wheelchair_bed/build

# Include any dependencies generated for this target.
include wheelchair_bed/CMakeFiles/wheelchair_bed.dir/depend.make

# Include the progress variables for this target.
include wheelchair_bed/CMakeFiles/wheelchair_bed.dir/progress.make

# Include the compile flags for this target's objects.
include wheelchair_bed/CMakeFiles/wheelchair_bed.dir/flags.make

wheelchair_bed/qrc_images.cxx: /home/aibot/wheelchair_bed/src/wheelchair_bed/resources/images/icon.png
wheelchair_bed/qrc_images.cxx: wheelchair_bed/resources/images.qrc.depends
wheelchair_bed/qrc_images.cxx: /home/aibot/wheelchair_bed/src/wheelchair_bed/resources/images.qrc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating qrc_images.cxx"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/lib/i386-linux-gnu/qt4/bin/rcc -name images -o /home/aibot/wheelchair_bed/build/wheelchair_bed/qrc_images.cxx /home/aibot/wheelchair_bed/src/wheelchair_bed/resources/images.qrc

wheelchair_bed/ui_main_window.h: /home/aibot/wheelchair_bed/src/wheelchair_bed/ui/main_window.ui
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating ui_main_window.h"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/lib/i386-linux-gnu/qt4/bin/uic -o /home/aibot/wheelchair_bed/build/wheelchair_bed/ui_main_window.h /home/aibot/wheelchair_bed/src/wheelchair_bed/ui/main_window.ui

wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx: /home/aibot/wheelchair_bed/src/wheelchair_bed/include/wheelchair_bed/main_window.hpp
wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx: wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx_parameters
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Generating include/wheelchair_bed/moc_main_window.cxx"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed && /usr/lib/i386-linux-gnu/qt4/bin/moc @/home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx_parameters

wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx: /home/aibot/wheelchair_bed/src/wheelchair_bed/include/wheelchair_bed/qnode.hpp
wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx: wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx_parameters
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Generating include/wheelchair_bed/moc_qnode.cxx"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed && /usr/lib/i386-linux-gnu/qt4/bin/moc @/home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx_parameters

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/flags.make
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o: /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main_window.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o -c /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main_window.cpp

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.i"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main_window.cpp > CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.i

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.s"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main_window.cpp -o CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.s

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.requires:

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.requires

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.provides: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.requires
	$(MAKE) -f wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build.make wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.provides.build
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.provides

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.provides.build: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o


wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/flags.make
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o: /home/aibot/wheelchair_bed/src/wheelchair_bed/src/qnode.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o -c /home/aibot/wheelchair_bed/src/wheelchair_bed/src/qnode.cpp

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.i"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aibot/wheelchair_bed/src/wheelchair_bed/src/qnode.cpp > CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.i

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.s"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aibot/wheelchair_bed/src/wheelchair_bed/src/qnode.cpp -o CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.s

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.requires:

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.requires

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.provides: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.requires
	$(MAKE) -f wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build.make wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.provides.build
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.provides

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.provides.build: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o


wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/flags.make
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o: /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wheelchair_bed.dir/src/main.cpp.o -c /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main.cpp

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wheelchair_bed.dir/src/main.cpp.i"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main.cpp > CMakeFiles/wheelchair_bed.dir/src/main.cpp.i

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wheelchair_bed.dir/src/main.cpp.s"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aibot/wheelchair_bed/src/wheelchair_bed/src/main.cpp -o CMakeFiles/wheelchair_bed.dir/src/main.cpp.s

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.requires:

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.requires

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.provides: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.requires
	$(MAKE) -f wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build.make wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.provides.build
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.provides

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.provides.build: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o


wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/flags.make
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o: wheelchair_bed/qrc_images.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o -c /home/aibot/wheelchair_bed/build/wheelchair_bed/qrc_images.cxx

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.i"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aibot/wheelchair_bed/build/wheelchair_bed/qrc_images.cxx > CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.i

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.s"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aibot/wheelchair_bed/build/wheelchair_bed/qrc_images.cxx -o CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.s

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.requires:

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.requires

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.provides: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.requires
	$(MAKE) -f wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build.make wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.provides.build
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.provides

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.provides.build: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o


wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/flags.make
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o: wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o -c /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.i"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx > CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.i

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.s"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx -o CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.s

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.requires:

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.requires

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.provides: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.requires
	$(MAKE) -f wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build.make wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.provides.build
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.provides

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.provides.build: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o


wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/flags.make
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o: wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o -c /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.i"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx > CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.i

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.s"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aibot/wheelchair_bed/build/wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx -o CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.s

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.requires:

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.requires

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.provides: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.requires
	$(MAKE) -f wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build.make wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.provides.build
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.provides

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.provides.build: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o


# Object files for target wheelchair_bed
wheelchair_bed_OBJECTS = \
"CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o" \
"CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o" \
"CMakeFiles/wheelchair_bed.dir/src/main.cpp.o" \
"CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o" \
"CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o" \
"CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o"

# External object files for target wheelchair_bed
wheelchair_bed_EXTERNAL_OBJECTS =

/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build.make
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libQtGui.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libQtCore.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/libroscpp.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_filesystem.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_signals.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/librosconsole.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/librosconsole_log4cxx.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/librosconsole_backend_interface.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/liblog4cxx.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_regex.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/libroscpp_serialization.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/libxmlrpcpp.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/librostime.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /opt/ros/kinetic/lib/libcpp_common.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_system.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_thread.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_chrono.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_date_time.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libboost_atomic.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libpthread.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: /usr/lib/i386-linux-gnu/libconsole_bridge.so
/home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aibot/wheelchair_bed/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX executable /home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed"
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wheelchair_bed.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build: /home/aibot/wheelchair_bed/devel/lib/wheelchair_bed/wheelchair_bed

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/build

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/requires: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main_window.cpp.o.requires
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/requires: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/qnode.cpp.o.requires
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/requires: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/src/main.cpp.o.requires
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/requires: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/qrc_images.cxx.o.requires
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/requires: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_main_window.cxx.o.requires
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/requires: wheelchair_bed/CMakeFiles/wheelchair_bed.dir/include/wheelchair_bed/moc_qnode.cxx.o.requires

.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/requires

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/clean:
	cd /home/aibot/wheelchair_bed/build/wheelchair_bed && $(CMAKE_COMMAND) -P CMakeFiles/wheelchair_bed.dir/cmake_clean.cmake
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/clean

wheelchair_bed/CMakeFiles/wheelchair_bed.dir/depend: wheelchair_bed/qrc_images.cxx
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/depend: wheelchair_bed/ui_main_window.h
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/depend: wheelchair_bed/include/wheelchair_bed/moc_main_window.cxx
wheelchair_bed/CMakeFiles/wheelchair_bed.dir/depend: wheelchair_bed/include/wheelchair_bed/moc_qnode.cxx
	cd /home/aibot/wheelchair_bed/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aibot/wheelchair_bed/src /home/aibot/wheelchair_bed/src/wheelchair_bed /home/aibot/wheelchair_bed/build /home/aibot/wheelchair_bed/build/wheelchair_bed /home/aibot/wheelchair_bed/build/wheelchair_bed/CMakeFiles/wheelchair_bed.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : wheelchair_bed/CMakeFiles/wheelchair_bed.dir/depend

