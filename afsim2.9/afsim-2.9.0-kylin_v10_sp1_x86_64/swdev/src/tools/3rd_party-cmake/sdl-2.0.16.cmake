# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
get_filename_component(CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
include(${CURRENT_LIST_DIR}/shared.cmake)
set(SDL2ROOT ${sdl_ROOT_DIR})
set(SDL2INC ${SDL2ROOT}/include)
set(SDL2LIBDIR ${SDL2ROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE})
set(SDL2DEBBINDIR ${SDL2ROOT}/debug/bin)
set(SDL2RELBINDIR ${SDL2ROOT}/release/bin)
set(SDL2LIB sdl2)

file(GLOB SDL2_INSTALL_LIBS ${SDLLIBDIR}/*.so*)
macro(link_sdl2 TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} debug ${SDL2ROOT}/debug/lib/sdl2d.lib optimized ${SDL2ROOT}/release/lib/sdl2.lib)
   else(WIN32)
      target_link_libraries(${TARGET} ${SDL2ROOT}/lib64/libSDL2.so)
   endif(WIN32)
endmacro(link_sdl2)

set(sdl2_INCLUDE_DIR ${SDL2ROOT}/include)
set_property(GLOBAL PROPERTY sdl_BINDIR_DEBUG ${SDL2ROOT}/debug/bin)
set_property(GLOBAL PROPERTY sdl_BINDIR_RELEASE ${SDL2ROOT}/release/bin)
if(WIN32)
   set(sdl2_LIBRARY debug ${SDL2ROOT}/debug/lib/sdl2d.lib optimized ${SDL2ROOT}/release/lib/sdl2.lib)
else()
   set(sdl2_LIBRARY ${SDL2ROOT}/lib64/libSDL2.so)
endif()
