// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvPlatformDataInterface.hpp"

#include <iomanip>
#include <set>
#include <sstream>

#include "RvEventPipeUtil.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtAirspeed.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"

RvPlatformData::Interface::Interface(QObject* parent)
   : QObject(parent)
   , mPlatformData()
   , mIndex(SCENARIO)
{
}

void RvPlatformData::Interface::SetPlatformOfInterest(const std::string& aPlatformName)
{
   mSideUpdater   = wkfEnv.GetUpdater("Side", aPlatformName);
   mTypeUpdater   = wkfEnv.GetUpdater("Type", aPlatformName);
   mIconUpdater   = wkfEnv.GetUpdater("Icon", aPlatformName);
   mDomainUpdater = wkfEnv.GetUpdater("Spatial Domain", aPlatformName);
   mStatusUpdater = wkfEnv.GetUpdater("Status", aPlatformName);

   QMutexLocker locker(&mMutex);
   mPlatformData.name = aPlatformName.c_str();
   mIndex             = LOOKUP_REQUIRED;
}

void RvPlatformData::Interface::AdvanceTimeRead(const rv::ResultData& aData)
{
   QMutexLocker locker(&mMutex);

   if (mIndex == LOOKUP_REQUIRED)
   {
      mIndex = aData.FindPlatformIndex(mPlatformData.name.toStdString());
   }

   if (mIndex > 0)
   {
      rv::ResultPlatform* platform = aData.FindPlatform(mIndex);
      double              simTime  = aData.GetSimTime();

      if (platform != nullptr)
      {
         platform->BeginRead();
         {
            mPlatformData.parts.clear();

            for (auto& part : platform->GetParts())
            {
               auto ps = part.FindFirstBefore<rv::MsgPartStatus>(simTime);
               if (nullptr != ps)
               {
                  QString ptype = rv::PartType::ToString(ps->partType());
                  if (ptype.isEmpty())
                  {
                     ptype = rv::MilPartType::ToString(ps->partType());
                  }

                  QString partSummary;
                  if (ps->on())
                     partSummary = "on";
                  else
                     partSummary = "off";
                  if (ps->disabled())
                     partSummary += ";disabled";
                  if (ps->broken())
                     partSummary += ";broken";
                  if ((ps->partType() == 0) && (ps->on()) && (!ps->disabled()) && (!ps->broken()))
                  {
                     std::string mode   = "default";
                     const auto* modeCh = part.FindFirstBefore<rv::MsgSensorModeChange>(simTime);
                     if (modeCh)
                     {
                        mode = modeCh->modeName();
                     }
                     if (!mode.empty())
                     {
                        partSummary += ";mode = ";
                        partSummary += QString::fromStdString(mode);
                     }
                  }
                  else if ((ps->partType() == rv::MilPartType::weapon) && (ps->on()) && (!ps->disabled()) &&
                           (!ps->broken()))
                  {
                     std::string                    modeName   = "default";
                     const rv::MsgWeaponModeChange* modeChange = part.FindFirstBefore<rv::MsgWeaponModeChange>(simTime);
                     if (modeChange != nullptr)
                     {
                        modeName = modeChange->modeName();
                     }
                     if (!modeName.empty())
                     {
                        partSummary += ";mode = ";
                        partSummary += QString::fromStdString(modeName);
                     }
                     auto quant = part.FindFirstBefore<rv::MsgWeaponQuantityChange>(simTime);
                     if (quant)
                     {
                        partSummary += ";quantity = ";
                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(2) << quant->weaponQuantity();
                        partSummary += ss.str().c_str();
                     }
                  }
                  mPlatformData.parts[ptype].insert(std::make_pair(ps->partName().c_str(), partSummary));
               }
            }
         }
         auto infoPtr = platform->FindFirstBefore<rv::MsgPlatformInfo>(simTime);
         if (infoPtr == nullptr)
         {
            infoPtr = platform->FindFirstAfter<rv::MsgPlatformInfo>(simTime);
         }
         platform->EndRead();

         if (nullptr != infoPtr)
         {
            mPlatformData.categories.clear();
            if (!infoPtr->categories().empty())
            {
               for (size_t i = 0; i < infoPtr->categories().size(); ++i)
               {
                  mPlatformData.categories.insert(infoPtr->categories()[(int)i].c_str());
               }
            }
         }
         if (mSideUpdater)
         {
            mPlatformData.side = mSideUpdater->GetValueString();
         }
         if (mIconUpdater)
         {
            mPlatformData.icon = mIconUpdater->GetValueString();
         }
         if (mDomainUpdater)
         {
            mPlatformData.spatial_domain = mDomainUpdater->GetValueString();
         }
         if (mTypeUpdater)
         {
            mPlatformData.type = mTypeUpdater->GetValueString();
         }
         if (mStatusUpdater)
         {
            mPlatformData.status = mStatusUpdater->GetValueString();
         }

         mPlatformData.startTime         = platform->GetStartTime();
         mPlatformData.endTime           = platform->GetEndTime();
         mPlatformData.fuelCapacity      = infoPtr->fuelCapacity();
         mPlatformData.fuelCapacityValid = infoPtr->fuelCapacityValid();

         auto statePtr = platform->FindFirstBefore<rv::MsgEntityState>(simTime);
         if (nullptr != statePtr)
         {
            mPlatformData.damageFactor = statePtr->state().damageFactor();
            mPlatformData.fuelCurrent  = statePtr->state().fuelCurrent();
            UtEntity tmpEntity;
            tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
            tmpEntity.SetOrientationWCS(statePtr->state().orientationWCS().x(),
                                        statePtr->state().orientationWCS().y(),
                                        statePtr->state().orientationWCS().z());
            tmpEntity.SetVelocityWCS(toVec3(statePtr->state().velocityWCS()).GetData());
            tmpEntity.SetAccelerationWCS(toVec3(statePtr->state().accelerationWCS()).GetData());

            double lat, lon, alt, wcs[3];
            tmpEntity.GetLocationLLA(lat, lon, alt);
            tmpEntity.GetLocationWCS(wcs);

            mPlatformData.latitude   = lat;
            mPlatformData.longitude  = lon;
            mPlatformData.altitude_m = alt;
            mPlatformData.x          = wcs[0];
            mPlatformData.y          = wcs[1];
            mPlatformData.z          = wcs[2];
            tmpEntity.GetOrientationNED(mPlatformData.yaw, mPlatformData.pitch, mPlatformData.roll);
            tmpEntity.GetOrientationWCS(mPlatformData.psi, mPlatformData.theta, mPlatformData.phi);
            mPlatformData.speed_m   = tmpEntity.GetSpeed();
            mPlatformData.machValid = statePtr->state().machNumberValid();
            if (statePtr->state().machNumberValid())
            {
               mPlatformData.mach = statePtr->state().machNumber();
            }

            double vned[3];
            double vwcs[3];
            tmpEntity.GetVelocityNED(vned);
            tmpEntity.GetVelocityWCS(vwcs);
            mPlatformData.vel_n   = vned[0];
            mPlatformData.vel_e   = vned[1];
            mPlatformData.vel_d   = vned[2];
            mPlatformData.vel_x   = vwcs[0];
            mPlatformData.vel_y   = vwcs[1];
            mPlatformData.vel_z   = vwcs[2];
            mPlatformData.accel_m = tmpEntity.GetAccelerationMagnitude();
            double aned[3];
            double awcs[3];
            tmpEntity.GetAccelerationNED(aned);
            tmpEntity.GetAccelerationWCS(awcs);
            mPlatformData.accel_n     = aned[0];
            mPlatformData.accel_e     = aned[1];
            mPlatformData.accel_d     = aned[2];
            mPlatformData.accel_x     = awcs[0];
            mPlatformData.accel_y     = awcs[1];
            mPlatformData.accel_z     = awcs[2];
            mPlatformData.state_valid = true;
         }
         else
         {
            mPlatformData.state_valid = false;
         }
      }
   }
   else
   {
   }
}

RvPlatformData::Interface::PlatformData RvPlatformData::Interface::GetPlatformData()
{
   QMutexLocker locker(&mMutex);
   return mPlatformData;
}
