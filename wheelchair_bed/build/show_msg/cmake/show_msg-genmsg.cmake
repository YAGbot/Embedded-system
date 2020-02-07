# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "show_msg: 1 messages, 0 services")

set(MSG_I_FLAGS "-Ishow_msg:/home/aibot/wheelchair_bed/src/show_msg/msg;-Istd_msgs:/opt/ros/kinetic/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(show_msg_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg" NAME_WE)
add_custom_target(_show_msg_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "show_msg" "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg" ""
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(show_msg
  "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/show_msg
)

### Generating Services

### Generating Module File
_generate_module_cpp(show_msg
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/show_msg
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(show_msg_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(show_msg_generate_messages show_msg_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg" NAME_WE)
add_dependencies(show_msg_generate_messages_cpp _show_msg_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(show_msg_gencpp)
add_dependencies(show_msg_gencpp show_msg_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS show_msg_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(show_msg
  "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/show_msg
)

### Generating Services

### Generating Module File
_generate_module_eus(show_msg
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/show_msg
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(show_msg_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(show_msg_generate_messages show_msg_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg" NAME_WE)
add_dependencies(show_msg_generate_messages_eus _show_msg_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(show_msg_geneus)
add_dependencies(show_msg_geneus show_msg_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS show_msg_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(show_msg
  "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/show_msg
)

### Generating Services

### Generating Module File
_generate_module_lisp(show_msg
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/show_msg
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(show_msg_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(show_msg_generate_messages show_msg_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg" NAME_WE)
add_dependencies(show_msg_generate_messages_lisp _show_msg_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(show_msg_genlisp)
add_dependencies(show_msg_genlisp show_msg_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS show_msg_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(show_msg
  "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/show_msg
)

### Generating Services

### Generating Module File
_generate_module_nodejs(show_msg
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/show_msg
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(show_msg_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(show_msg_generate_messages show_msg_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg" NAME_WE)
add_dependencies(show_msg_generate_messages_nodejs _show_msg_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(show_msg_gennodejs)
add_dependencies(show_msg_gennodejs show_msg_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS show_msg_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(show_msg
  "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/show_msg
)

### Generating Services

### Generating Module File
_generate_module_py(show_msg
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/show_msg
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(show_msg_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(show_msg_generate_messages show_msg_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/aibot/wheelchair_bed/src/show_msg/msg/info.msg" NAME_WE)
add_dependencies(show_msg_generate_messages_py _show_msg_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(show_msg_genpy)
add_dependencies(show_msg_genpy show_msg_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS show_msg_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/show_msg)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/show_msg
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_cpp)
  add_dependencies(show_msg_generate_messages_cpp std_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/show_msg)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/show_msg
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_eus)
  add_dependencies(show_msg_generate_messages_eus std_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/show_msg)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/show_msg
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_lisp)
  add_dependencies(show_msg_generate_messages_lisp std_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/show_msg)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/show_msg
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_nodejs)
  add_dependencies(show_msg_generate_messages_nodejs std_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/show_msg)
  install(CODE "execute_process(COMMAND \"/usr/bin/python2\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/show_msg\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/show_msg
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_py)
  add_dependencies(show_msg_generate_messages_py std_msgs_generate_messages_py)
endif()
