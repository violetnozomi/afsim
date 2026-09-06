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

#ifndef WSFATMOSPHERETYPES_HPP
#define WSFATMOSPHERETYPES_HPP

#include "wsf_space_export.h"

#include "WsfAtmosphere.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace space
{

class AtmosphereTypes : public WsfObjectTypeList<wsf::space::Atmosphere>
{
public:
   static WSF_SPACE_EXPORT AtmosphereTypes& Get(const WsfScenario& aScenario);

   WSF_SPACE_EXPORT explicit AtmosphereTypes(WsfScenario& aScenario);
};

} // namespace space
} // namespace wsf

#endif // WSFATMOSPHEREMODELTYPES_HPP
