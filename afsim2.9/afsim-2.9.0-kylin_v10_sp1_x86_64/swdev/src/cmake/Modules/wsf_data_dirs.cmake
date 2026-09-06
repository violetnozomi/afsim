# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# demos
if(NOT WSF_DEMOS_ROOT)
   find_path(WSF_DEMOS_ROOT demos .. ../.. NO_DEFAULT_PATH)
   if(IS_DIRECTORY "${WSF_DEMOS_ROOT}")
      set(WSF_DEMOS_ROOT "${WSF_DEMOS_ROOT}/demos" CACHE PATH "WSF demos directory" FORCE)
   endif()
endif()

if(WSF_DEMOS_ROOT AND IS_DIRECTORY "${WSF_DEMOS_ROOT}")
   file(GLOB ALL_WSF_DEMOS_DIRS ${WSF_DEMOS_ROOT}/*
        PATTERN .git EXCLUDE
        PATTERN ".*.swo" EXCLUDE
        PATTERN ".*.swp" EXCLUDE
        PATTERN .* EXCLUDE
       )

   if(COMMAND add_wsf_doc_input AND EXISTS "${WSF_DEMOS_ROOT}/doc" AND WSF_INSTALL_DEMOS)
      add_wsf_doc_input(${WSF_DEMOS_ROOT}/doc)
   endif()
endif()

# demos_shared
if(NOT WSF_DEMOS_SHARED_ROOT)
   find_path(WSF_DEMOS_SHARED_ROOT demos_shared .. ../.. NO_DEFAULT_PATH)
   if(IS_DIRECTORY "${WSF_DEMOS_SHARED_ROOT}")
      set(WSF_DEMOS_SHARED_ROOT "${WSF_DEMOS_SHARED_ROOT}/demos_shared" CACHE PATH "WSF demos_shared directory" FORCE)
   endif()
endif()

if(WSF_DEMOS_SHARED_ROOT AND IS_DIRECTORY "${WSF_DEMOS_SHARED_ROOT}")
   file(GLOB ALL_WSF_DEMOS_SHARED_DIRS ${WSF_DEMOS_SHARED_ROOT}/*
        PATTERN .git EXCLUDE
        PATTERN ".*.swo" EXCLUDE
        PATTERN ".*.swp" EXCLUDE
        PATTERN .* EXCLUDE
       )
endif()

# scenarios
if(NOT WSF_SCENARIOS_ROOT)
   find_path(WSF_SCENARIOS_ROOT scenarios .. ../.. NO_DEFAULT_PATH)
   if(IS_DIRECTORY "${WSF_SCENARIOS_ROOT}")
      set(WSF_SCENARIOS_ROOT "${WSF_SCENARIOS_ROOT}/scenarios" CACHE PATH "WSF scenarios directory" FORCE)
   endif()
endif()

if(WSF_SCENARIOS_ROOT AND IS_DIRECTORY "${WSF_SCENARIOS_ROOT}")
   file(GLOB ALL_WSF_SCENARIOS_DIRS ${WSF_SCENARIOS_ROOT}/*
        PATTERN .git EXCLUDE
        PATTERN ".*.swo" EXCLUDE
        PATTERN ".*.swp" EXCLUDE
        PATTERN .* EXCLUDE
       )
endif()

# scenarios_shared
if(NOT WSF_SCENARIOS_SHARED_ROOT)
   find_path(WSF_SCENARIOS_SHARED_ROOT scenarios_shared .. ../.. NO_DEFAULT_PATH)
   if(IS_DIRECTORY "${WSF_SCENARIOS_SHARED_ROOT}")
      set(WSF_SCENARIOS_SHARED_ROOT "${WSF_SCENARIOS_SHARED_ROOT}/scenarios_shared" CACHE PATH "WSF scenarios_shared directory" FORCE)
   endif()
endif()

if(WSF_SCENARIOS_SHARED_ROOT AND IS_DIRECTORY "${WSF_SCENARIOS_SHARED_ROOT}")
   file(GLOB ALL_WSF_SCENARIOS_SHARED_DIRS ${WSF_SCENARIOS_SHARED_ROOT}/*
        PATTERN .git EXCLUDE
        PATTERN ".*.swo" EXCLUDE
        PATTERN ".*.swp" EXCLUDE
        PATTERN .* EXCLUDE
       )
endif()

# training
if(NOT WSF_TRAINING_ROOT)
   find_path(WSF_TRAINING_ROOT training .. ../.. NO_DEFAULT_PATH)
   if(IS_DIRECTORY "${WSF_TRAINING_ROOT}")
      set(WSF_TRAINING_ROOT "${WSF_TRAINING_ROOT}/training" CACHE PATH "WSF training directory" FORCE)
   endif()
endif()

if(WSF_TRAINING_ROOT AND IS_DIRECTORY "${WSF_TRAINING_ROOT}")
   file(GLOB ALL_WSF_TRAINING_DIRS ${WSF_TRAINING_ROOT}/*
        PATTERN .git EXCLUDE
        PATTERN ".*.swo" EXCLUDE
        PATTERN ".*.swp" EXCLUDE
        PATTERN .* EXCLUDE
       )

   if(COMMAND add_wsf_doc_input AND EXISTS "${WSF_TRAINING_ROOT}/doc")
      add_wsf_doc_input(${WSF_TRAINING_ROOT}/doc)
   endif()
endif()

# tools
if(NOT WSF_TOOLS_ROOT)
   find_path(WSF_TOOLS_ROOT tools .. ../.. NO_DEFAULT_PATH)
   if(IS_DIRECTORY "${WSF_TOOLS_ROOT}")
      set(WSF_TOOLS_ROOT "${WSF_TOOLS_ROOT}/tools" CACHE PATH "WSF tools directory" FORCE)
   endif()
endif()

if(WSF_TOOLS_ROOT AND IS_DIRECTORY "${WSF_TOOLS_ROOT}")
   file(GLOB ALL_WSF_TOOLS_DIRS ${WSF_TOOLS_ROOT}/*
        PATTERN .git EXCLUDE
        PATTERN ".*.swo" EXCLUDE
        PATTERN ".*.swp" EXCLUDE
        PATTERN .* EXCLUDE
       )
	   
   if(COMMAND add_wsf_doc_input AND EXISTS "${WSF_TOOLS_ROOT}/doc")
      add_wsf_doc_input(${WSF_TOOLS_ROOT}/doc)
   endif()
endif()
