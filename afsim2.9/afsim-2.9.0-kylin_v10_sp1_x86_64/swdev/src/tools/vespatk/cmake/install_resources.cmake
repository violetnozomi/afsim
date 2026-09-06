# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
macro(swdev_install_resources DEST)
   set(RESOURCES_TARGET_DIR "${DEST}/resources")
   install(SCRIPT "${afsim_SOURCE_DIR}/tools/vespatk/cmake/resources.cmake")
   # Copy resource contents; the source directory trailing slash prevents including source directory name.
   install(DIRECTORY
         "${VTK_DEV_RESOURCES_PATH}/"
         "${afsim_SOURCE_DIR}/tools/vespatk/data"
      DESTINATION "${RESOURCES_TARGET_DIR}" COMPONENT Resource
      PATTERN "*.timestamp" EXCLUDE
      PATTERN "site" EXCLUDE
      PATTERN ".git*" EXCLUDE)
   install(DIRECTORY "${afsim_SOURCE_DIR}/tools/vespatk/shaders"
      "${afsim_SOURCE_DIR}/tools/vespatk/models"
      DESTINATION "${RESOURCES_TARGET_DIR}" COMPONENT Runtime)
endmacro()
