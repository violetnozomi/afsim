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
#ifndef WSFPLATFORMAVAILABILITY_HPP
#define WSFPLATFORMAVAILABILITY_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

#include "WsfStringId.hpp"
class UtInput;
class WsfPlatform;
class WsfSimulation;

//! Determines platform availability from a list of criteria specified in the input file
//! If a platform is not available, the simulation will not create the platform.
class WSF_EXPORT WsfPlatformAvailability
{
public:
   WsfPlatformAvailability();
   bool PlatformIsAvailable(WsfPlatform* aPlatformPtr, WsfSimulation& aSimulation) const;

   bool ProcessInput(UtInput& aInput);

   void SetDefaultAvailability(double aProbability) { mDefaultAvailability = aProbability; }

   void SetAvailabilityByName(WsfStringId aName, double aAvailability) { mAvailabilityByName[aName] = aAvailability; }

private:
   //! Platform Availability
   //@{
   //! The operational availability of platforms by name.
   std::map<WsfStringId, double> mAvailabilityByName;

   //! The operational availability of platforms by type.
   std::map<WsfStringId, double> mAvailabilityByType;

   //! The operational availability of platforms by type.
   std::map<WsfStringId, double> mAvailabilityByCategory;

   //! Exclusion zones for operational availability.
   std::vector<WsfStringId> mExclusionZones;

   //! Inclusion zones for operational availability.
   std::vector<WsfStringId> mInclusionZones;

   //! The default operational availability
   double mDefaultAvailability;
   //@}
};
#endif
