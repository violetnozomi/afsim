# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by Infoscitex, a DCS Company.
# ****************************************************************************
project(pack_to_cpp)
include_directories(../../source $<TARGET_PROPERTY:util,INTERFACE_INCLUDE_DIRECTORIES>)

REMOVE_DEFINITIONS(-Dutil_EXPORTS -Dwsf_util_EXPORTS -DSWDEV_ALL_USE_DLL)
add_definitions(-DUT_STATIC_DEFINE)
add_definitions(-DWSF_UT_STATIC_DEFINE)

if(NOT TOOLS_DIRECTORY)
   set(TOOLS_DIRECTORY ../../../../tools)
endif()

add_executable(${PROJECT_NAME} pack_to_cpp.cpp
   # including dependencies from util directly
   # otherwise any change to util will force an update in pack_to_cpp
   # which in turn updates generated code, even if no change is necessary
   ${TOOLS_DIRECTORY}/util/source/UtTextDocument.cpp
   ${TOOLS_DIRECTORY}/util/source/UtStringUtil.cpp
   ${TOOLS_DIRECTORY}/util/source/UtPath.cpp
   ${TOOLS_DIRECTORY}/util/source/UtCallback.cpp
   ${TOOLS_DIRECTORY}/util/source/UtLog.cpp
   ${TOOLS_DIRECTORY}/util/source/UtLogBatch.cpp
   ${TOOLS_DIRECTORY}/util/source/UtLogMessage.cpp
   ${TOOLS_DIRECTORY}/util/source/UtLogPublisher.cpp
   ${TOOLS_DIRECTORY}/util/source/UtLogStream.cpp
   ${TOOLS_DIRECTORY}/util/source/UtLogSubscriber.cpp
   ${TOOLS_DIRECTORY}/util/source/UtTypeInfo.cpp
   ../../source/UtBuffer.cpp
   ../../source/Utml.cpp
   ../../source/UtPackSchema.cpp
   )

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "core")
