# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_grammar_check)

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   source/WsfGrammarCheck.hpp
SRCS
   source/WsfGrammarCheck.cpp
NOTICES
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_parser)