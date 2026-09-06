plugin_cmakelists_template(NetworkResourceManager warlock
   LIBRARIES warlock_core wsf_network_resource_manager
   NO_INSTALL_SOURCE
)

target_include_directories(NetworkResourceManager PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/../../include
   ${CMAKE_CURRENT_SOURCE_DIR}/../../source
)

target_sources(NetworkResourceManager PRIVATE
   NrmAcceptanceDemoPanel.cpp
   NrmAcceptanceDemoPanel.hpp
   NrmUiScale.cpp
   NrmUiScale.hpp
   NrmUiStyle.cpp
   NrmUiStyle.hpp
   NrmUiText.cpp
   NrmUiText.hpp
   NrmCustomerJsonCodec.cpp
   NrmCustomerJsonCodec.hpp
   NrmDemandDetailTabs.cpp
   NrmDemandDetailTabs.hpp
   NrmOperatorPresentation.cpp
   NrmOperatorPresentation.hpp
   NrmOperationalDetailTabs.cpp
   NrmOperationalDetailTabs.hpp
   NrmPreacceptanceStatusMonitor.cpp
   NrmPreacceptanceStatusMonitor.hpp
   NrmPlanScenarioBinding.cpp
   NrmPlanScenarioBinding.hpp
   NrmSnapshotReporter.cpp
   NrmSnapshotReporter.hpp
   NrmTacticalActivity.hpp
   NrmTacticalSelection.cpp
   NrmTacticalSelection.hpp
   NrmTacticalViewport.cpp
   NrmTacticalViewport.hpp
   NrmTacticalView.cpp
   NrmTacticalView.hpp
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
