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

#include "RvOrbitalDataInterface.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtEarth.hpp"
#include "UtOrbitalState.hpp"

RvOrbitalData::Interface::Interface(QObject* aParentPtr)
   : QObject(aParentPtr)
   , mOrbitalData()
   , mIndex(cINVALID_INDEX)
   , mOrbitalVisible(false)
{
}

/** Set the platform of interest, to see data about it. */
void RvOrbitalData::Interface::SetPlatformOfInterest(const std::string& aPlatformName)
{
   mOrbitalData.mName = aPlatformName.c_str();
   mIndex             = cLOOKUP_REQUIRED;
}

/** Advance time and read new data. */
void RvOrbitalData::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   // If we need to do a lookup, do one.
   if (mIndex == cLOOKUP_REQUIRED)
   {
      mIndex = aData.FindPlatformIndex(mOrbitalData.mName);
   }

   // If we have a platform, read the data
   if (mIndex > 0)
   {
      rv::ResultPlatform* platform = aData.FindPlatform(mIndex);
      if (platform)
      {
         auto stateMsgPtr = platform->FindFirstBefore<rv::MsgEntityState>(aData.GetSimTime());
         auto infoMsgPtr  = platform->FindFirstBefore<rv::MsgPlatformInfo>(aData.GetSimTime());
         mOrbitalVisible  = (infoMsgPtr && infoMsgPtr->spatialDomain() == rv::SpatialDomain::space);
         if (mOrbitalVisible && stateMsgPtr)
         {
            mOrbitalData       = std::move(GenerateOrbitalData(stateMsgPtr->state(), aData.GetSimTime()));
            mOrbitalData.mName = platform->GetName();
         }
      }
   }
}

/** Generates an OrbitalDataContainer, using the entity's state at a specific simulation time. */
RvOrbitalData::OrbitalDataContainer RvOrbitalData::Interface::GenerateOrbitalData(const rv::EntityState& aState,
                                                                                  double                 aSimTime)
{
   auto cal = rvEnv.GetEpoch();
   cal.AdvanceTimeBy(aSimTime);

   UtECI_Conversion converter(cal, ut::EarthEGM96());
   converter.SetLocationWCS(UtVec3d(aState.locationWCS().x(), aState.locationWCS().y(), aState.locationWCS().z()));
   converter.SetVelocityWCS(UtVec3d(aState.velocityWCS().x(), aState.velocityWCS().y(), aState.velocityWCS().z()));

   ut::OrbitalState orbitalState(ut::EarthEGM96(),
                                 ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                 ut::OrbitalState::ReferenceFrame::cECI);
   orbitalState.SetInertial(cal, ut::OrbitalStateVector(converter.GetLocationECI(), converter.GetVelocityECI()));
   const auto& orbitalElements = orbitalState.GetOrbitalElementsTOD();

   RvOrbitalData::OrbitalDataContainer data;
   data.mEccentricity   = orbitalElements.GetEccentricity();
   data.mSemiMajorAxis  = orbitalElements.GetSemiMajorAxis();
   data.mInclination    = orbitalElements.GetInclination();
   data.mRAAN           = orbitalElements.GetRAAN();
   data.mArgOfPeriapsis = orbitalElements.GetArgumentOfPeriapsis();
   data.mTrueAnomaly    = orbitalElements.GetTrueAnomaly();
   return data;
}

/** Get the data container */
const RvOrbitalData::OrbitalDataContainer RvOrbitalData::Interface::GetOrbitalData()
{
   return mOrbitalData;
}

/** Get if Orbital Data is being populated. */
bool RvOrbitalData::Interface::IsOrbitalVisible()
{
   return mOrbitalVisible;
}
