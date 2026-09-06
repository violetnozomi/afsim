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
project(genio)
include(GenerateExportHeader)
include(swdev_project)
SET (GENIO_WITHOUT_UMP TRUE)


FILE(GLOB SRC *.cpp *.c)
FILE(GLOB HDR *.hpp *.h)

IF(CMAKE_COMPILER_IS_GNUCXX)
   # This argument causes the linker to exit if the library has unresolved
   # symbols.  Do not disable this option.
   SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
ENDIF()

IF((DEFINED GENIO_WITHOUT_UMP) AND (GENIO_WITHOUT_UMP))
   # Building genio without UMP
   # Find and remove code that is ump related
   FILE(GLOB UMP_SRCS GenUmp* GenLoggerNet*)
   FOREACH(UMP_SRC ${UMP_SRCS})
      LIST(REMOVE_ITEM SRC ${UMP_SRC})
      LIST(REMOVE_ITEM HDR ${UMP_SRC})
   ENDFOREACH(UMP_SRC ${UMP_SRCS})
   ADD_LIBRARY(${PROJECT_NAME} ${SRC} ${HDR})
ELSE()
   # Building genio using UMP
   FIND_PACKAGE(ump REQUIRED)
   INCLUDE_DIRECTORIES(${ump_INCLUDE_DIRS})
   LINK_DIRECTORIES(${ump_LIBRARY_DIRS})
   ADD_LIBRARY(${PROJECT_NAME} ${SRC} ${HDR})
   TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ump_LIBRARIES})
ENDIF()

generate_export_header(${PROJECT_NAME})
if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")

target_link_libraries(${PROJECT_NAME} util)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

link_sockets(${PROJECT_NAME})
swdev_lib_install(${PROJECT_NAME})
swdev_warning_level(${PROJECT_NAME})
install_sources(../source tools/${PROJECT_NAME})
