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

#include "RvOrbitalDataUpdaters.hpp"

#include "RvEnvironment.hpp"
#include "RvOrbitalDataInterface.hpp"
#include "RvResultPlatform.hpp"
#include "UtEarth.hpp"
#include "UtOrbitalState.hpp"

bool RvOrbitalData::EccentricityUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto stateMsgPtr    = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   auto infoMsgPtr     = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   bool hasDomainSpace = (infoMsgPtr && infoMsgPtr->spatialDomain() == rv::SpatialDomain::space);
   if (hasDomainSpace && stateMsgPtr)
   {
      auto data = std::move(RvOrbitalData::Interface::GenerateOrbitalData(stateMsgPtr->state(), aSimTime));
      mValue    = data.mEccentricity;
      return true;
   }
   return false;
}

bool RvOrbitalData::SemiMajorAxisUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto stateMsgPtr    = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   auto infoMsgPtr     = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   bool hasDomainSpace = (infoMsgPtr && infoMsgPtr->spatialDomain() == rv::SpatialDomain::space);
   if (hasDomainSpace && stateMsgPtr)
   {
      auto data = std::move(RvOrbitalData::Interface::GenerateOrbitalData(stateMsgPtr->state(), aSimTime));
      mValue    = data.mSemiMajorAxis;
      return true;
   }
   return false;
}

bool RvOrbitalData::InclinationUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto stateMsgPtr    = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   auto infoMsgPtr     = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   bool hasDomainSpace = (infoMsgPtr && infoMsgPtr->spatialDomain() == rv::SpatialDomain::space);
   if (hasDomainSpace && stateMsgPtr)
   {
      auto data = std::move(RvOrbitalData::Interface::GenerateOrbitalData(stateMsgPtr->state(), aSimTime));
      mValue    = data.mInclination;
      return true;
   }
   return false;
}

bool RvOrbitalData::RAAN_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto stateMsgPtr    = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   auto infoMsgPtr     = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   bool hasDomainSpace = (infoMsgPtr && infoMsgPtr->spatialDomain() == rv::SpatialDomain::space);
   if (hasDomainSpace && stateMsgPtr)
   {
      auto data = std::move(RvOrbitalData::Interface::GenerateOrbitalData(stateMsgPtr->state(), aSimTime));
      mValue    = data.mRAAN;
      return true;
   }
   return false;
}

bool RvOrbitalData::ArgOfPeriapsisUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto stateMsgPtr    = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   auto infoMsgPtr     = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   bool hasDomainSpace = (infoMsgPtr && infoMsgPtr->spatialDomain() == rv::SpatialDomain::space);
   if (hasDomainSpace && stateMsgPtr)
   {
      auto data = std::move(RvOrbitalData::Interface::GenerateOrbitalData(stateMsgPtr->state(), aSimTime));
      mValue    = data.mArgOfPeriapsis;
      return true;
   }
   return false;
}

bool RvOrbitalData::TrueAnomalyUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto stateMsgPtr    = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   auto infoMsgPtr     = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   bool hasDomainSpace = (infoMsgPtr && infoMsgPtr->spatialDomain() == rv::SpatialDomain::space);
   if (hasDomainSpace && stateMsgPtr)
   {
      auto data = std::move(RvOrbitalData::Interface::GenerateOrbitalData(stateMsgPtr->state(), aSimTime));
      mValue    = data.mTrueAnomaly;
      return true;
   }
   return false;
}
