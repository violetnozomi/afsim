# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

set(TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}/../Templates")

macro(create_vs_debug_env EXE_TARGET_DIRECTORY
                          USER_FILE_TEMPLATE)
   if (MSVC_IDE)
      set(DEBUG_PATH "%PATH%")
      set(RELEASE_PATH "%PATH%")


      get_property(PKGS GLOBAL PROPERTY SWDEV_ALL_PACKAGES_USED)
      if (NOT "${EXE_TARGET_DIRECTORY}" STREQUAL "")
         get_property(PKGS_LOCAL DIRECTORY ${EXE_TARGET_DIRECTORY} PROPERTY SWDEV_ALL_PACKAGES_USED)
         list(APPEND PKGS "${PKGS_LOCAL}")
      endif()

      # Until CMake v3.14 (cmake commit 121a036f7) an error is generated if the list argument doesn't exist
      if(PKGS)
         list(REMOVE_DUPLICATES PKGS)
      endif()

      foreach(pkg ${PKGS})
         get_property(${pkg}_BINDIR_DEBUG GLOBAL PROPERTY ${pkg}_BINDIR_DEBUG)
         foreach(pkgBinDir ${${pkg}_BINDIR_DEBUG})
            set(DEBUG_PATH "${pkgBinDir}\;${DEBUG_PATH}")
         endforeach()

         get_property(${pkg}_BINDIR_RELEASE GLOBAL PROPERTY ${pkg}_BINDIR_RELEASE)
         foreach(pkgBinDir ${${pkg}_BINDIR_RELEASE})
            set(RELEASE_PATH "${pkgBinDir}\;${RELEASE_PATH}")
         endforeach()
      endforeach()
      set(DEBUG_ENVIRONMENT "SOURCE_ROOT=${TOOLS_DIRECTORY}/..\nPATH=${DEBUG_PATH}\nRESOURCE_PATH=${VTK_DEV_RESOURCES_PATH}")
      set(RELEASE_ENVIRONMENT "SOURCE_ROOT=${TOOLS_DIRECTORY}/..\nPATH=${RELEASE_PATH}\nRESOURCE_PATH=${VTK_DEV_RESOURCES_PATH}")
      string(REPLACE "/" "\\" DEBUG_ENVIRONMENT ${DEBUG_ENVIRONMENT})
      string(REPLACE "/" "\\" RELEASE_ENVIRONMENT ${RELEASE_ENVIRONMENT})

      set(VCPROJ_OUT_DIR ${PROJECT_BINARY_DIR})
      if(${CMAKE_VERSION} VERSION_LESS "3.13.0")
         configure_file(${USER_FILE_TEMPLATE} ${VCPROJ_OUT_DIR}/${PROJECT_NAME}.vcxproj.user)
      else()
         set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "$<$<CONFIG:Debug>:${DEBUG_ENVIRONMENT}>$<$<CONFIG:Release>:${RELEASE_ENVIRONMENT}>$<$<CONFIG:MinSizeRel>:${RELEASE_ENVIRONMENT}>$<$<CONFIG:RelWithDebInfo>:${RELEASE_ENVIRONMENT}>")
      endif()
   endif (MSVC_IDE)
endmacro()

# Deprecated compatibility - function instead of macro to encapsulate args as variables.
function(write_vcproj_user
   EXE_TARGET_DIRECTORY
   USER_FILE_TEMPLATE
   VCPROJ_OUT_DIRECTORY
   ADDITIONAL_PATH_DEBUG
   ADDITIONAL_PATH_RELEASE
   ADDITIONAL_ENVIRONMENT_DEBUG
   ADDITIONAL_ENVIRONMENT_RELEASE)

   message(DEPRECATION "The write_vcproj_user macro was deprecated in AFSIM 2.9, migrate to create_vs_debug_env.")

   set(_unsupported_args
      VCPROJ_OUT_DIRECTORY
      ADDITIONAL_PATH_DEBUG
      ADDITIONAL_PATH_RELEASE
      ADDITIONAL_ENVIRONMENT_DEBUG
      ADDITIONAL_ENVIRONMENT_RELEASE
   )

   set(_unsupported_arg_provided OFF)
   foreach(_old_arg IN LISTS _unsupported_args)
      if(NOT "${${_old_arg}}" STREQUAL "")
         message(SEND_ERROR
            "The write_vcproj_user macro does not provide backwards compatibility for the ${_old_arg} parameter!")
         set(_unsupported_arg_provided ON)
      endif()
   endforeach()

   if(_unsupported_arg_provided)
      return()
   else()
      # Call replacement macro when it's a suitable standin.
      create_vs_debug_env("${EXE_TARGET_DIRECTORY}" "${USER_FILE_TEMPLATE}")
   endif()
endfunction()
