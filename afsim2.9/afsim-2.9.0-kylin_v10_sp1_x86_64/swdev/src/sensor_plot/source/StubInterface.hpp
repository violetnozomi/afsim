// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef STUBINTERFACE_HPP
#define STUBINTERFACE_HPP

#include <string>
#include <vector>

#include "UtCallbackHolder.hpp"
#include "WsfScenarioExtension.hpp"
class UtInput;

//! An interface that provides a mechanism to implement stubs for missing features.
//!
//! When sensor_plot is built, many 'unnecessary' options may not be provided. For instance,
//! the missile models CSIMS and JAAM are needed in 'wsf', but aren't really needed in sensor_plot.
//! However, if one wants to do a horizontal_map of an actual scenario, the scenario may include
//! definitions that use these 'unnecessary' options and thus cause an input error.
//!
//! Rather than make the user modify the scenarios simply to run with sensor_plot, this provides
//! a mechanism to define commands and object types that will simply be ignored.
class StubInterface : public WsfScenarioExtension
{
public:
   StubInterface(WsfScenario& aScenario);
   ~StubInterface() override;

   bool ProcessInput(UtInput& aInput) override;

private:
   std::vector<std::string> mIgnoredBlocks;
   std::vector<std::string> mIgnoredLines;
   std::vector<std::string> mIgnoredWords;
};

#endif
