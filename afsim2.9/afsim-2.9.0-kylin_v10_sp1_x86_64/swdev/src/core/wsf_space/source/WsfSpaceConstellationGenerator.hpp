// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACECONSTELLATIONGENERATOR_HPP
#define WSFSPACECONSTELLATIONGENERATOR_HPP

#include "wsf_space_export.h"

#include <ostream>

class WsfConstellationOptions;

namespace wsf
{
namespace space
{

namespace ConstellationGenerator
{

void WSF_SPACE_EXPORT Generate(const WsfConstellationOptions& aOptions, std::ostream& aStream);

} // namespace ConstellationGenerator

} // namespace space
} // namespace wsf

#endif // WSFSPACECONSTELLATIONGENERATOR_HPP
