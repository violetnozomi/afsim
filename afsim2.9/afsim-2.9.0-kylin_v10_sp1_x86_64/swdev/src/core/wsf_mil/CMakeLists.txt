# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(wsf_mil)

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/include)
add_subdirectory(source)
#add_subdirectory(test)

# Add source directories to doxygen input
#add_wsf_doxygen_input(${SRCS}) TODO Use this instead? Adds all source files
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/comm)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/dis)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/ew)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/mover)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/observer)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/processor)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/script)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/sensor)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/weapon)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/xio)

# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})

# INSTALL
install_sources(source core/${PROJECT_NAME})
install_sources_all_files(grammar core/${PROJECT_NAME})
install_sources_all_files(doc core/${PROJECT_NAME})
install_sources_all_files(test core/${PROJECT_NAME})
install_tests(test_mission core/${PROJECT_NAME})
install_source_files(CMakeLists.txt
                     README.md
                     wsf_module
                     wsf_cmake_extension.cmake
                     wsf_mil_events.utpack
                     core/${PROJECT_NAME}
                    )

if(WSF_INSTALL_DEMOS)
   # Data directories ALWAYS included with mil build
   set(WSF_MIL_DEMO_DIRS
       acoustic
       aea_iads
       ballistic
       ballistic_missile_shootdown
       base_types
       bearing_only
       behavior_tree
       ciws
       distributed_operations
       draw
       electronic_warfare
       example_scripts
       gun_engagement
       heatmap
       hel
       hellfire
       iads
       kinematic_mover
       laser_designator
       launcher
       logistics
       new_guidance
       noise_cloud
       orwaca_iads
       oth_radar
       outer_air_battle
       parachute
       route_finder_demos
       script_demos
       sensor_demos
       sensor_plot
       ship_ad
       shooter
       signature_demos
       simple_scenario
       swarm
       tbm_demos
       terrain_following
       timeline
       timeline_lead
       traffic_demos
       visual_part
      )
   install_wsf_data("${WSF_MIL_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()

if(WSF_INSTALL_TRAINING)
   # Training directories ALWAYS included with mil build
   set(WSF_USER_TRAIN_MIL_DIRS
       1_AFSIM_Intro
       2_Platforms_and_Movers
       3_Weapons
       4_Sensors
       5_Scripting
       6_Task_Processors_and_Comms
      )
   install_wsf_data("${WSF_USER_TRAIN_MIL_DIRS}" ${WSF_TRAINING_ROOT}/user training/user Training)
   install_wsf_data(10_Other_Topics/bam ${WSF_TRAINING_ROOT}/user training/user/10_Other_Topics Training)

   add_wsf_doc_input(${WSF_TRAINING_ROOT}/user/doc)

   set(WSF_DEV_TRAIN_MIL_DIRS
       core
      )
   install_wsf_data("${WSF_DEV_TRAIN_MIL_DIRS}" ${WSF_TRAINING_ROOT}/developer training/developer Training)
endif()
