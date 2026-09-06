// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFNONEXPORTABLEBASE_HPP
#define WSFNONEXPORTABLEBASE_HPP

#include <iosfwd>

#include "WsfSimulationExtension.hpp"

class WsfNonExportableBase : public WsfSimulationExtension
{
public:
   ~WsfNonExportableBase() override;
};

#endif
