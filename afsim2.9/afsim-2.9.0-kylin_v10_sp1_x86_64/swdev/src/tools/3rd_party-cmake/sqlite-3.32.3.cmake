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
set(SQLITEROOT ${sqlite_ROOT_DIR})
set(SQLITEINC ${SQLITEROOT}/include)
set(SQLITELIBDIR ${SQLITEROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE})
set(SQLITELIB sqlite3)
set(SQLITEDEBBINDIR ${SQLITEROOT}/debug)
set(SQLITERELBINDIR ${SQLITEROOT}/release)

macro(link_sqlite TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} debug ${SQLITEROOT}/debug/sqlite3.lib optimized ${SQLITEROOT}/release/sqlite3.lib)
   else(WIN32)
      target_link_libraries(${TARGET} ${SQLITEROOT}/lib/libsqlite3.a)
   endif(WIN32)
endmacro(link_sqlite)

set(sqlite_INCLUDE_DIR ${SQLITEROOT}/include)
set(sqlite_BINDIR_DEBUG ${SQLITEROOT}/debug)
set(sqlite_BINDIR_RELEASE ${SQLITEROOT}/release)
if(WIN32)
   set(sqlite_LIBRARY debug ${SQLITEROOT}/debug/sqlite3.lib optimized ${SQLITEROOT}/release/sqlite3.lib)
else()
   set(sqlite_LIBRARY ${SQLITEROOT}/lib/libsqlite3.a)
endif()
