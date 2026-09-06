//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "WsfSpaceKinematicStateExtrapolation.hpp"

#include "UtCalendar.hpp"
#include "UtCloneablePtr.hpp"
#include "UtECI_Conversion.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalState.hpp"
#include "WsfDateTime.hpp"
#include "WsfEnvironment.hpp"
#include "WsfFilter.hpp"
#include "WsfKeplerianOrbitalPropagator.hpp"
#include "WsfMeasurement.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfTrack.hpp"

namespace wsf
{

namespace space
{
std::unique_ptr<UtAttribute<KinematicState>> KinematicStateExtrapolation::Extrapolate(double                aSimTime,
                                                                                      const WsfMeasurement& aMeasurement) const
{
   auto ksiPtr = ut::make_unique<wsf::KinematicStateInformation>();

   // If a stable filter is present use the filter to return the estimate.
   WsfFilter* filterPtr = const_cast<WsfFilter*>(aMeasurement.GetFilter());
   if ((filterPtr != nullptr) && filterPtr->IsStable())
   {
      WsfMeasurement dataOut;
      filterPtr->GetKinematicEstimate(aSimTime, dataOut);
      if (dataOut.mVelocityWCS.IsValid())
      {
         ksiPtr->Get().SetLocationWCS(dataOut.mLocationWCS.Get());
         ksiPtr->Get().SetVelocityWCS(dataOut.mVelocityWCS.Get());
      }
   }
   else // Attempt to use measurement data to generate a valid location.
   {
      // Need both position and velocity to create a propagator
      if (aMeasurement.LocationValid() && aMeasurement.VelocityValid())
      {
#undef GetCurrentTime
         auto measurementEpoch = aMeasurement.GetSimulation()->GetDateTime().GetCurrentTime(aMeasurement.GetUpdateTime());
         if ((mOrbitalPropagatorPtr == nullptr) ||
             (measurementEpoch.GetJ2000_Date() > mOrbitalPropagatorPtr->GetInitialOrbitalState().GetEpoch().GetJ2000_Date()))
         {
            if (UpdatePropagator(measurementEpoch, aMeasurement))
            {
#undef GetCurrentTime
               auto updateTime = aMeasurement.GetSimulation()->GetDateTime().GetCurrentTime(aSimTime);
               mOrbitalPropagatorPtr->Update(updateTime);
               auto osvWCS = mOrbitalPropagatorPtr->GetOrbitalState().GetOrbitalStateVectorWCS();
               ksiPtr->Get().SetLocationWCS(osvWCS.GetLocation());
               ksiPtr->Get().SetVelocityWCS(osvWCS.GetVelocity());
            }
            else
            {
               ksiPtr->Get().SetLocationWCS(aMeasurement.GetLocationWCS());
               ksiPtr->Get().SetLocationWCS(aMeasurement.GetVelocityWCS());
            }
         }
      }
      else if (aMeasurement.LocationValid())
      {
         ksiPtr->Get().SetLocationWCS(aMeasurement.GetLocationWCS());
      }
      else if (aMeasurement.RangeValid() && (aMeasurement.BearingValid() && aMeasurement.ElevationValid()))
      {
         UtVec3d locWCS;
         UtMeasurementUtil::LocationWCS(aMeasurement.GetOriginatorLocationWCS().GetData(),
                                        aMeasurement.GetRange(),
                                        aMeasurement.GetBearing(),
                                        aMeasurement.GetElevation(),
                                        locWCS.GetData());
         ksiPtr->Get().SetLocationWCS(locWCS);
      }
      else
      {
         return nullptr;
      }
   }
   return std::move(ksiPtr);
}

//! Add or update a propagator to provide extrapolation.
bool KinematicStateExtrapolation::UpdatePropagator(const UtCalendar&     aMeasurementEpoch,
                                                   const WsfMeasurement& aMeasurement) const
{
   // Eventually, platforms will have their own central bodies, and the parent platform will need to be discovered from
   // 'aMeasurement'.
   UtECI_Conversion conv(aMeasurementEpoch, aMeasurement.GetSimulation()->GetEnvironment().GetCentralBody());

   if (mOrbitalPropagatorPtr == nullptr)
   {
      if (aMeasurement.IsTrack())
      {
         auto targetPlatformPtr =
            aMeasurement.GetSimulation()->GetPlatformByIndex(static_cast<const WsfTrack&>(aMeasurement).GetTargetIndex());
         if (targetPlatformPtr != nullptr)
         {
            auto smbPtr = dynamic_cast<WsfSpaceMoverBase*>(targetPlatformPtr->GetMover());
            if (smbPtr != nullptr)
            {
               // Use the target propagator.
               mOrbitalPropagatorPtr = ut::clone(smbPtr->GetPropagator());
               conv.SetCentralPoint(mOrbitalPropagatorPtr->GetCentralPoint());
            }
            else
            {
               // Target mover not a space mover; use a Keplerian propagator.
               mOrbitalPropagatorPtr = ut::make_unique<wsf::space::KeplerianOrbitalPropagator>();
            }
         }
      }
      else
      {
         // Target platform not identified; use a Keplerian propagator.
         mOrbitalPropagatorPtr = ut::make_unique<wsf::space::KeplerianOrbitalPropagator>();
      }
   }

   conv.SetLocationWCS(aMeasurement.GetLocationWCS());
   conv.SetVelocityWCS(aMeasurement.GetVelocityWCS());
   ut::OrbitalState orbitalState(aMeasurementEpoch,
                                 conv.GetCentralPoint(),
                                 ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cECI,
                                 ut::OrbitalStateVector(conv.GetLocationECI(), conv.GetVelocityECI()));
   return mOrbitalPropagatorPtr->Initialize(orbitalState);
}

} // namespace space
} // namespace wsf
