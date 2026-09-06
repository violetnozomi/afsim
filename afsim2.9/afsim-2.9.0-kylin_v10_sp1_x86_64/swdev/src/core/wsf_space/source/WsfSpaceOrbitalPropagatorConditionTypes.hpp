// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACEORBITALPROPAGATORCONDITIONTYPES_HPP
#define WSFSPACEORBITALPROPAGATORCONDITIONTYPES_HPP

#include "wsf_space_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfSpaceOrbitalPropagatorCondition.hpp"

namespace wsf
{
namespace space
{
class OrbitalPropagatorConditionTypes : public WsfObjectTypeList<OrbitalPropagatorCondition>
{
public:
   static WSF_SPACE_EXPORT OrbitalPropagatorConditionTypes& Get(const WsfScenario& aScenario);

   WSF_SPACE_EXPORT explicit OrbitalPropagatorConditionTypes(WsfScenario& aScenario);
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEORBITALPROPAGATORCONDITIONTYPES_HPP
