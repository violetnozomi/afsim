# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# Read a file and produce a C header file with a string containing the file's contents.

# Ex:  # Add a rule to generate the header file
#      compile_resource_to_c(cMY_RESOURCE_DATA  my_resource.dat  ${CMAKE_BINARY_DIR}/my_resource.h)
#      # Add the header file to the project
#      add_executable(xyz   ${CMAKE_BINARY_DIR}/my_resource.h)
macro(compile_resource_to_c C_VARIABLE_NAME RESFILE SRC_FILE_OUT)
   add_custom_command(OUTPUT ${SRC_FILE_OUT}
      COMMAND pack_to_cpp -quiet -res ${C_VARIABLE_NAME} "${RESFILE}" "${SRC_FILE_OUT}"
      DEPENDS "${RESFILE}" pack_to_cpp
     )
endmacro()

