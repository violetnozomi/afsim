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

#include "WsfEM_ALARM_Attenuation.hpp"

#include <cmath>

#include "UtVec3.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"

namespace WsfEM_ALARM_Attenuation
{
atmosphere::atmosphere(WsfEM_Xmtr* aXmtrPtr)
   : mXmtrPtr(aXmtrPtr)
   , mTempPlatform(nullptr)
{
}

double atmosphere::attenuation(double elevation, double frequency, double range, double rkfact)
{
   // NOTE: It is assume this is being called form the clutter model and that the radar is on the ground.

   double             value    = 1.0;
   WsfEM_Attenuation* modelPtr = mXmtrPtr->GetAttenuationModel();
   if (modelPtr != nullptr)
   {
      WsfSimulation* simPtr = mXmtrPtr->GetSimulation();
      if (mTempPlatform == nullptr)
      {
         mTempPlatform = new WsfPlatform(simPtr->GetScenario());
         mTempPlatform->AssignToSimulation(simPtr);
      }
      WsfEM_Interaction interaction;
      interaction.BeginGenericInteraction(mXmtrPtr, mTempPlatform, nullptr);
      WsfEM_Antenna&                   antenna = *(mXmtrPtr->GetAntenna());
      WsfEM_Interaction::LocationData& xmtrLoc = interaction.mXmtrLoc;
      antenna.GetLocationWCS(xmtrLoc.mLocWCS);
      antenna.GetLocationLLA(xmtrLoc.mLat, xmtrLoc.mLon, xmtrLoc.mAlt);

      // The target location is not really needed. All that is required is that the target altitude be
      // less than the source (transmitter) altitude to force the proper branch to be taken in
      // WsfEM_Interaction::GetRangeElevationAltitude().

      interaction.mTgtLoc.mAlt = xmtrLoc.mAlt - 1.0;

      // Compute the unit vector to the target.
      WsfEM_Interaction::RelativeData& xmtrToTgt = interaction.mXmtrToTgt;
      xmtrToTgt.mRange                           = range;
      xmtrToTgt.mAz                              = 0.0;
      xmtrToTgt.mTrueAz                          = 0.0;
      xmtrToTgt.mEl                              = 0.0;
      xmtrToTgt.mTrueEl                          = 0.0;
      double d                                   = -sin(elevation);
      double relLocNED[3]                        = {d, 0.0, sqrt(1.0 - d * d)};
      antenna.ConvertNED_VectorToWCS(relLocNED, xmtrToTgt.mUnitVecWCS);
      UtVec3d::Set(xmtrToTgt.mTrueUnitVecWCS, xmtrToTgt.mUnitVecWCS);

      value = modelPtr->ComputeAttenuationFactor(interaction,
                                                 simPtr->GetScenario().GetEnvironment(),
                                                 WsfEM_Interaction::cXMTR_TO_TARGET);
   }
   return value;
}

atmosphere::~atmosphere()
{
   delete mTempPlatform;
}


double attenuation(atmosphere& atm, double elevation, double frequency, double range, double rkfact)
{
   return atm.attenuation(elevation, frequency, range, rkfact);
}

} // namespace WsfEM_ALARM_Attenuation
