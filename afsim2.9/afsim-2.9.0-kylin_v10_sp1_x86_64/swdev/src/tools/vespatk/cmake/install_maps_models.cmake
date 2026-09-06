# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# Macros to install CME maps and icons

if (NOT VESPA_BINARY_DATA_ROOT)
   if(WIN32)
      set(VESPA_BINARY_DATA_ROOT //VESPA/cme_data)
      if(NOT IS_DIRECTORY ${VESPA_BINARY_DATA_ROOT})
        if(IS_DIRECTORY "//sllabl1/data/resources")
            set(VESPA_BINARY_DATA_ROOT "//sllabl1/data/resources")
        elseif(IS_DIRECTORY "//homer/l1r/data/resources")
            set(VESPA_BINARY_DATA_ROOT "//homer/l1r/data/resources")
        else()
           set(VESPA_BINARY_DATA_ROOT "${CMAKE_SOURCE_DIR}/../../dependencies/resources")
       endif()
      endif()
   else()
      set(VESPA_BINARY_DATA_ROOT //VESPA/cme_data)
      if(NOT IS_DIRECTORY ${VESPA_BINARY_DATA_ROOT})
        if(IS_DIRECTORY "/data/resources")
            set(VESPA_BINARY_DATA_ROOT "/data/resources")
       else()
          set(VESPA_BINARY_DATA_ROOT "${CMAKE_SOURCE_DIR}/../../dependencies/resources")
       endif()
      endif()
   endif()
endif()

macro(install_maps DEST)
   install(DIRECTORY "${VESPA_BINARY_DATA_ROOT}/maps" DESTINATION ${DEST} COMPONENT Resource)
endmacro()

macro(install_models DEST)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/AAA_README" DESTINATION ${DEST} COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/Boeing_Facets_Models_License_Agreement_11-23-09.pdf" DESTINATION ${DEST} COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/dis_map.dat" DESTINATION ${DEST} COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/fs_map.dat" DESTINATION ${DEST} COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/model_def.dat" DESTINATION ${DEST} COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/model_map.dat" DESTINATION ${DEST} COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/warhead_def.dat" DESTINATION ${DEST} COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/milstd/MilstdSymbols.ive" DESTINATION ${DEST}/milstd COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/milstd/MilstdBlack.rgb" DESTINATION ${DEST}/milstd COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/milstd/MilstdIconToSwitchMap.csv" DESTINATION ${DEST}/milstd COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/milstd/MilStdSymbols.flt" DESTINATION ${DEST}/milstd COMPONENT Resource)
   install(FILES     "${VESPA_BINARY_DATA_ROOT}/models/milstd/MilstdColors.color" DESTINATION ${DEST}/milstd COMPONENT Resource)
   install(DIRECTORY "${VESPA_BINARY_DATA_ROOT}/models/3d" DESTINATION ${DEST} COMPONENT Resource)
   install(DIRECTORY "${VESPA_BINARY_DATA_ROOT}/models/icons" DESTINATION ${DEST} COMPONENT Resource)
   install(DIRECTORY "${VESPA_BINARY_DATA_ROOT}/models/symbology" DESTINATION ${DEST} COMPONENT Resource)
   install(DIRECTORY "${VESPA_BINARY_DATA_ROOT}/p_effect" DESTINATION ${DEST} COMPONENT Resource)
endmacro()
