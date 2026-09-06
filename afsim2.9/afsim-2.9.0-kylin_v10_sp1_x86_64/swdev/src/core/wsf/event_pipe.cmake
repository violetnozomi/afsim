# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

get_filename_component(WSF_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/wsf PATH)
include(${WSF_SOURCE_DIR}/../wsf_util/source/UtResourceCompiler.cmake)
#
# Create custom build rules to convert a schema into C++ code
#
#   Results in these artifacts:
#    1. XyzClasses.hpp
#        the structs from the schema realized as C++ classes
#    2. XyzClassesRegister.hpp
#        registration code to be invoked once, registering the classes with a UtPackSerializer
#    3. XyzSchema.hpp
#        a header file containing the schema text as a C string
#        this is needed at runtime, and is more convenient to compile it in than read from a file
#
# SCHEMA_FILE - the file containing the schema text
# SCHEMA_DEPENDS - list of other schema files that are dependencies
# MESSAGE_BASE - The name of the message base class.
# MESSAGE_BASE_FILE - The file containing the message base class
# NAMESPACE - A namespace to put the generated classes in
# BASE_NAME - Base name of the generated files (Xyz above)
# SOURCES_VARIABLES - Name of the CMake variable to receive the new files
#                    these should be part of the add_library() call
MACRO(add_event_pipe_schema SCHEMA_FILE SCHEMA_DEPENDS MESSAGE_BASE MESSAGE_BASE_FILE NAMESPACE BASE_NAME INCLUDES_VARIABLE SOURCES_VARIABLE)
   file(MAKE_DIRECTORY "${afsim_BINARY_DIR}/include")

   set(CLASS_HEADER "${afsim_BINARY_DIR}/include/${BASE_NAME}Classes.hpp")
   set(CLASS_SOURCE "${afsim_BINARY_DIR}/include/${BASE_NAME}Classes.cpp")
   set(REGISTER_HEADER "${afsim_BINARY_DIR}/include/${BASE_NAME}ClassesRegister.hpp")
   set(SCHEMA_HEADER "${afsim_BINARY_DIR}/include/${BASE_NAME}Schema.hpp")

   if(NOT ${MESSAGE_BASE} STREQUAL "")
      set(MESSAGE_BASE_ARG "-msg-base" ${MESSAGE_BASE})
   endif()
   if(NOT ${MESSAGE_BASE_FILE} STREQUAL "")
      set(MESSAGE_BASE_FILE_ARG "-msg-base-file" ${MESSAGE_BASE_FILE})
   endif()

   add_custom_command(OUTPUT ${CLASS_HEADER} ${CLASS_SOURCE}
      COMMAND pack_to_cpp -quiet ${MESSAGE_BASE_ARG} ${MESSAGE_BASE_FILE_ARG} -namespace ${NAMESPACE} -h "${SCHEMA_FILE}" ${SCHEMA_DEPENDS} ${CLASS_HEADER}_temp
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CLASS_HEADER}_temp" "${CLASS_HEADER}"
      DEPENDS "${SCHEMA_FILE}" ${SCHEMA_DEPENDS} pack_to_cpp
      COMMENT "**** Generating ${CLASS_HEADER}"
   )

   add_custom_command(OUTPUT ${REGISTER_HEADER}
      COMMAND pack_to_cpp -quiet -class-header ${BASE_NAME}Classes.hpp -namespace ${NAMESPACE} -c "${SCHEMA_FILE}" ${SCHEMA_DEPENDS} ${REGISTER_HEADER}_temp
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${REGISTER_HEADER}_temp" "${REGISTER_HEADER}"
      DEPENDS "${SCHEMA_FILE}" ${SCHEMA_DEPENDS} pack_to_cpp
      COMMENT "**** Generating ${REGISTER_HEADER}"
   )

   compile_resource_to_c("c${BASE_NAME}_SCHEMA" "${SCHEMA_FILE}" "${SCHEMA_HEADER}")

   set(${INCLUDES_VARIABLE} "${CLASS_HEADER}" "${REGISTER_HEADER}" "${SCHEMA_HEADER}")
   set(${SOURCES_VARIABLE} "${CLASS_SOURCE}")
ENDMACRO()

