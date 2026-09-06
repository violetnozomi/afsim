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

#ifndef WSFSPACECONSTELLATIONCONJUNCTION_HPP
#define WSFSPACECONSTELLATIONCONJUNCTION_HPP

#include "wsf_space_export.h"

namespace wsf
{
namespace space
{

namespace ConstellationConjunction
{

bool WSF_SPACE_EXPORT Assess(int aNumPlanes, int aSatsPerPlane, double aInclination, double aRAAN_Range, double aAnomalyAlias);

} // namespace ConstellationConjunction

} // namespace space
} // namespace wsf

#endif // WSFSPACECONSTELLATIONCONJUNCTION_HPP
