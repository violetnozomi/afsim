// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvPlatformDataUpdater.hpp"

#include "RvEventPipeUtil.hpp"
#include "RvResultPlatform.hpp"
#include "UtAirspeed.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMath.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

bool RvPlatformData::AltitudeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      double lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(toVec3(statePtr->state().locationWCS()).GetData(), lla[0], lla[1], lla[2]);
      mValue = lla[2];
   }
   return statePtr != nullptr;
}

double RvPlatformData::HeadingUpdater::GetValueInDefaultUnits() const
{
   return UtMath::NormalizeAngle0_TwoPi(mValue);
}

bool RvPlatformData::HeadingUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      tmpEntity.SetOrientationWCS(statePtr->state().orientationWCS().x(),
                                  statePtr->state().orientationWCS().y(),
                                  statePtr->state().orientationWCS().z());

      double pitch, roll;
      tmpEntity.GetOrientationNED(mValue, pitch, roll);
   }
   return statePtr != nullptr;
}

bool RvPlatformData::PitchUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      tmpEntity.SetOrientationWCS(statePtr->state().orientationWCS().x(),
                                  statePtr->state().orientationWCS().y(),
                                  statePtr->state().orientationWCS().z());

      double heading, roll;
      tmpEntity.GetOrientationNED(heading, mValue, roll);
   }
   return statePtr != nullptr;
}

bool RvPlatformData::RollUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      tmpEntity.SetOrientationWCS(statePtr->state().orientationWCS().x(),
                                  statePtr->state().orientationWCS().y(),
                                  statePtr->state().orientationWCS().z());

      double heading, pitch;
      tmpEntity.GetOrientationNED(heading, pitch, mValue);
   }
   return statePtr != nullptr;
}

bool RvPlatformData::SpeedUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      rv::Vec3f v = statePtr->state().velocityWCS();
      mValue      = sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
   }
   return statePtr != nullptr;
}

bool RvPlatformData::LatitudeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      double lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(toVec3(statePtr->state().locationWCS()).GetData(), mValue, lla[1], lla[2]);
   }
   return statePtr != nullptr;
}

bool RvPlatformData::LongitudeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      double lla[3];
      UtEllipsoidalEarth::ConvertECEFToLLA(toVec3(statePtr->state().locationWCS()).GetData(), lla[0], mValue, lla[2]);
   }
   return statePtr != nullptr;
}

QString RvPlatformData::PlatformTypeUpdater::GetValueString() const
{
   return QString::fromStdString(mPlatformType);
}

bool RvPlatformData::PlatformTypeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto infoPtr = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   if (infoPtr == nullptr)
   {
      infoPtr = aPlatform->FindFirstAfter<rv::MsgPlatformInfo>(aSimTime);
   }
   if (infoPtr != nullptr)
   {
      if (!infoPtr->types().empty())
      {
         mPlatformType = *infoPtr->types().begin();
      }
   }
   return true;
}

QString RvPlatformData::PositionUpdater::GetValueString() const
{
   UtLatPos latitude(mLatitude_deg, wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
   UtLonPos longitude(mLongitude_deg, wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());

   std::ostringstream stream;
   stream << latitude << " " << longitude;
   return QString::fromStdString(stream.str());
}

bool RvPlatformData::PositionUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      double alt;
      UtEllipsoidalEarth::ConvertECEFToLLA(toVec3(statePtr->state().locationWCS()).GetData(),
                                           mLatitude_deg,
                                           mLongitude_deg,
                                           alt);
   }
   return statePtr != nullptr;
}

bool RvPlatformData::LocationX_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().locationWCS().x();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::LocationY_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().locationWCS().y();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::LocationZ_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().locationWCS().z();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::PsiUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().orientationWCS().x();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::ThetaUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().orientationWCS().y();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::PhiUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().orientationWCS().z();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::VelocityN_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      double v[3] = {statePtr->state().velocityWCS().x(),
                     statePtr->state().velocityWCS().y(),
                     statePtr->state().velocityWCS().z()};
      tmpEntity.SetVelocityWCS(v);
      tmpEntity.GetVelocityNED(v);

      mValue = v[0];
   }
   return statePtr != nullptr;
}

bool RvPlatformData::VelocityE_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      double v[3] = {statePtr->state().velocityWCS().x(),
                     statePtr->state().velocityWCS().y(),
                     statePtr->state().velocityWCS().z()};
      tmpEntity.SetVelocityWCS(v);
      tmpEntity.GetVelocityNED(v);

      mValue = v[1];
   }
   return statePtr != nullptr;
}

bool RvPlatformData::VelocityD_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      double v[3] = {statePtr->state().velocityWCS().x(),
                     statePtr->state().velocityWCS().y(),
                     statePtr->state().velocityWCS().z()};
      tmpEntity.SetVelocityWCS(v);
      tmpEntity.GetVelocityNED(v);

      mValue = v[2];
   }
   return statePtr != nullptr;
}

bool RvPlatformData::VelocityX_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().velocityWCS().x();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::VelocityY_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().velocityWCS().y();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::VelocityZ_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().velocityWCS().z();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::MachUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      if (statePtr->state().machNumberValid())
      {
         mValue = statePtr->state().machNumber();
         return true;
      }
   }
   return false;
}

bool RvPlatformData::AccelerationUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      rv::Vec3f a = statePtr->state().accelerationWCS();
      mValue      = sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
   }
   return statePtr != nullptr;
}

bool RvPlatformData::AccelerationN_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      double a[3] = {statePtr->state().accelerationWCS().x(),
                     statePtr->state().accelerationWCS().y(),
                     statePtr->state().accelerationWCS().z()};
      tmpEntity.SetAccelerationWCS(a);
      tmpEntity.GetAccelerationNED(a);

      mValue = a[0];
   }
   return statePtr != nullptr;
}

bool RvPlatformData::AccelerationE_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      double a[3] = {statePtr->state().accelerationWCS().x(),
                     statePtr->state().accelerationWCS().y(),
                     statePtr->state().accelerationWCS().z()};
      tmpEntity.SetAccelerationWCS(a);
      tmpEntity.GetAccelerationNED(a);

      mValue = a[1];
   }
   return statePtr != nullptr;
}

bool RvPlatformData::AccelerationD_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      UtEntity tmpEntity;
      tmpEntity.SetLocationWCS(toVec3(statePtr->state().locationWCS()).GetData());
      double a[3] = {statePtr->state().accelerationWCS().x(),
                     statePtr->state().accelerationWCS().y(),
                     statePtr->state().accelerationWCS().z()};
      tmpEntity.SetAccelerationWCS(a);
      tmpEntity.GetAccelerationNED(a);

      mValue = a[2];
   }
   return statePtr != nullptr;
}

bool RvPlatformData::AccelerationX_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().accelerationWCS().x();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::AccelerationY_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().accelerationWCS().y();
   }
   return statePtr != nullptr;
}

bool RvPlatformData::AccelerationZ_Updater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().accelerationWCS().z();
   }
   return statePtr != nullptr;
}

QString RvPlatformData::PlatformSideUpdater::GetValueString() const
{
   return QString::fromStdString(mSide);
}

bool RvPlatformData::PlatformSideUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto infoPtr = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   if (infoPtr == nullptr)
   {
      infoPtr = aPlatform->FindFirstAfter<rv::MsgPlatformInfo>(aSimTime);
   }
   if (infoPtr != nullptr)
   {
      mSide = infoPtr->side();
   }
   return true;
}

QString RvPlatformData::PlatformIconUpdater::GetValueString() const
{
   return QString::fromStdString(mIcon);
}

bool RvPlatformData::PlatformIconUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto infoPtr = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   if (infoPtr == nullptr)
   {
      infoPtr = aPlatform->FindFirstAfter<rv::MsgPlatformInfo>(aSimTime);
   }
   if (infoPtr != nullptr)
   {
      mIcon = infoPtr->icon();
   }
   return true;
}

QString RvPlatformData::PlatformDomainUpdater::GetValueString() const
{
   return QString::fromStdString(mDomain);
}

bool RvPlatformData::PlatformDomainUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto infoPtr = aPlatform->FindFirstBefore<rv::MsgPlatformInfo>(aSimTime);
   if (infoPtr == nullptr)
   {
      infoPtr = aPlatform->FindFirstAfter<rv::MsgPlatformInfo>(aSimTime);
   }
   if (infoPtr != nullptr)
   {
      mDomain = rv::SpatialDomain::ToString(infoPtr->spatialDomain());
   }
   return true;
}

QString RvPlatformData::PlatformStatusUpdater::GetValueString() const
{
   return QString::fromStdString(mStatus);
}

bool RvPlatformData::PlatformStatusUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   auto statusPtr = aPlatform->FindFirstBefore<rv::MsgPlatformStatus>(aSimTime);
   auto statePtr  = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);

   if (statusPtr != nullptr)
   {
      bool changed = false;
      if (statusPtr->broken())
      {
         mStatus = "broken";
         changed = true;
      }
      if (statusPtr->removed())
      {
         if (changed)
         {
            mStatus += " & removed";
         }
         else
         {
            mStatus = "removed";
         }
         changed = true;
      }
      if (!changed)
      {
         mStatus = "live";
      }
   }
   else if (statePtr != nullptr)
   {
      mStatus = "live";
   }
   else
   {
      mStatus = "unloaded";
   }
   return true;
}

bool RvPlatformData::StartTimeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   mValue = aPlatform->GetStartTime();
   return true;
}

bool RvPlatformData::EndTimeUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   mValue = aPlatform->GetEndTime();
   return true;
}

bool RvPlatformData::DamageFactorUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().damageFactor();
   }
   return statePtr != nullptr;
}
bool RvPlatformData::FuelUpdater::ReadData(rv::ResultPlatform* aPlatform, double aSimTime)
{
   const auto statePtr = aPlatform->FindFirstBefore<rv::MsgEntityState>(aSimTime);
   if (statePtr != nullptr)
   {
      mValue = statePtr->state().fuelCurrent();
   }
   return statePtr != nullptr;
}
