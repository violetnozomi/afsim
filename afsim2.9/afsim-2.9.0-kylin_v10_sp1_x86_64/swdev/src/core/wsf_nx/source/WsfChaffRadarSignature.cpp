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
#include "WsfChaffRadarSignature.hpp"

#include <cassert>

#include "UtLog.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"

WsfChaffRadarSignature::WsfChaffRadarSignature()
   : WsfRadarSignature()
   , mParcelPtr(nullptr)
{
}

WsfRadarSignature* WsfChaffRadarSignature::Clone() const
{
   return new WsfChaffRadarSignature(*this);
}

float WsfChaffRadarSignature::GetSignature(WsfStringId               aStateId,
                                           WsfEM_Types::Polarization aPolarization,
                                           double                    aFrequency,
                                           double                    aTgtToXmtrAz,
                                           double                    aTgtToXmtrEl,
                                           double                    aTgtToRcvrAz,
                                           double                    aTgtToRcvrEl,
                                           WsfEM_Xmtr*               aXmtrPtr /*= nullptr*/,
                                           WsfEM_Rcvr*               aRcvrPtr /*= nullptr*/)
{
   assert(aXmtrPtr && aRcvrPtr);
   double xmtrLocWCS[3];
   aXmtrPtr->GetAntenna()->GetLocationWCS(xmtrLocWCS);
   double rcvrLocWCS[3];
   aRcvrPtr->GetAntenna()->GetLocationWCS(rcvrLocWCS);
   return static_cast<float>(mParcelPtr->GetRadarCrossSection(aFrequency, xmtrLocWCS, rcvrLocWCS));
}

std::pair<float, float> WsfChaffRadarSignature::GetSignatureLimits(WsfStringId               aStateId,
                                                                   WsfEM_Types::Polarization aPolarization) const
{
   return {0.0f, static_cast<float>(mParcelPtr->GetMaximumRCS())};
}

bool WsfChaffRadarSignature::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (!WsfChaffParcel::Find(mParcelPtr, *aPlatformPtr))
   {
      auto logger = ut::log::warning() << "Chaff radar signature could not find parcel extension on platform.";
      logger.AddNote() << "Using default radar signature.";
      logger.AddNote() << "Platform: " << aPlatformPtr->GetName();
      logger.AddNote() << "Type: " << aPlatformPtr->GetType();
      return false;
   }

   return true;
}
