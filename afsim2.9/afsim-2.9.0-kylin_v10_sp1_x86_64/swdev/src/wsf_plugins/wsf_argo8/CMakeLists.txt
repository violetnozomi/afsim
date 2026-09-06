# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(swdev_project)
#swdev_is_64bit(IS_64BIT)

project(wsf_argo8)

add_subdirectory(argo8/source argo8)
set_property(TARGET argo8 PROPERTY FOLDER "wsf_argo8")

add_subdirectory(source)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "wsf_argo8")

# Add source directories to doxygen input
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)

# Add project to documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})

# INSTALL
install_sources_all_files(. wsf_plugins/${PROJECT_NAME})
# find pre-built shared libraries
if(UNIX)
   FILE(GLOB ORIG_DEBUG_SHARED_LIBS   argo8/models/*/*.so) # find debug so files
   FILE(GLOB ORIG_RELEASE_SHARED_LIBS argo8/models/*/*.so) # find release so files
else()
   FILE(GLOB ORIG_DEBUG_SHARED_LIBS   argo8/models/*/*.dll) # find debug dll files
   FILE(GLOB ORIG_RELEASE_SHARED_LIBS argo8/models/*/*.dll) # find release dll files
endif()


set(ARGO8_INSTALL_PATH ${INSTALL_DLL_PATH})

if(WIN32)
   install(FILES ${ORIG_DEBUG_SHARED_LIBS}
           CONFIGURATIONS Debug
           DESTINATION ${ARGO8_INSTALL_PATH}
           COMPONENT Runtime)

   install(FILES ${ORIG_RELEASE_SHARED_LIBS}
           CONFIGURATIONS Release
           DESTINATION ${ARGO8_INSTALL_PATH}
           COMPONENT Runtime)
else()
   install(FILES ${ORIG_DEBUG_SHARED_LIBS}
           CONFIGURATIONS Debug
           DESTINATION ${ARGO8_INSTALL_PATH}
           COMPONENT Runtime)

   install(FILES ${ORIG_RELEASE_SHARED_LIBS}
           CONFIGURATIONS Release
           DESTINATION ${ARGO8_INSTALL_PATH}
           COMPONENT Runtime)
endif()

if(WSF_INSTALL_SCENARIOS)
   set(ARGO8_DATA_SECRET_DIRS
       argo8
       argo8_types
      )
   install_wsf_data("${ARGO8_DATA_SECRET_DIRS}" ${WSF_SCENARIOS_ROOT} scenarios Scenario)
endif()
