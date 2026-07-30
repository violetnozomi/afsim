plugin_cmakelists_template(NetworkResourceManager warlock
   LIBRARIES warlock_core
   NO_INSTALL_SOURCE
)

target_include_directories(NetworkResourceManager PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

if(UNIX)
   add_custom_command(TARGET NetworkResourceManager POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory
              ${CMAKE_BINARY_DIR}/warlock_plugins
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:NetworkResourceManager>
              ${CMAKE_BINARY_DIR}/warlock_plugins/$<TARGET_FILE_NAME:NetworkResourceManager>
      COMMENT "Staging NetworkResourceManager for a development Warlock run"
   )
endif()
