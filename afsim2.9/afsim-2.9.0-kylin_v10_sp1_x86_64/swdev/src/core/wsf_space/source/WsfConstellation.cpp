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

#include "WsfConstellation.hpp"

#include "WsfConstellationManager.hpp"
#include "WsfSimulation.hpp"

WsfConstellation::WsfConstellation(const WsfConstellationManager& aManager, const WsfConstellationOptions& aOptions)
   : mOptions{aOptions}
   , mManager(aManager)
{
}

//! Return the name of a constellation member.
//!
//! Return the name of the member at the given @p aPlane and at the given position,
//! @p aMember. This returns the name of what the member would be whether or not that
//! member currently exists in the simulation.
//!
//! \param aPlane  - The orbital plane of the member.
//! \param aMember - The position in the given @p aPlane of the member.
//! \return        - The name of the member platform that would take that position in the constellation.
std::string WsfConstellation::GetMemberName(unsigned int aPlane, unsigned int aMember) const
{
   return GetName() + "_" + std::to_string(aPlane) + "_" + std::to_string(aMember);
}

//! Return a constellation member platform.
//!
//! Return the WsfPlatform for the constellation member in the given @p aPlane and
//! given @p aMember position in that plane. This will return either the member
//! platform if it exists, or a nullptr, if that member platform does not exist.
//!
//! \param aPlane  - The orbital plane of the member.
//! \param aMember - The position in the given @p aPlane of the member.
//! \return        - The platform if it exists; nullptr otherwise.
WsfPlatform* WsfConstellation::GetMember(unsigned int aPlane, unsigned int aMember) const
{
   std::string name = GetMemberName(aPlane, aMember);
   return mManager.GetSimulation().GetPlatformByName(name);
}
