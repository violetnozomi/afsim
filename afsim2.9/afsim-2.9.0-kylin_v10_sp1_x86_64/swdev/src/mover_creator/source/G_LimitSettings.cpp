// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "G_LimitSettings.hpp"

#include <stdexcept>

#include <QJsonObject>

namespace
{
   constexpr double cDEFAULT_AIRCRAFT_PITCH_G_LOAD_MAX =  8.0;
   constexpr double cDEFAULT_AIRCRAFT_PITCH_G_LOAD_MIN = -2.0;
   constexpr double cDEFAULT_AIRCRAFT_YAW_G_LOAD_MAX   =  0.5;

   constexpr double cDEFAULT_WEAPON_PITCH_G_LOAD_MAX =  60.0;
   constexpr double cDEFAULT_WEAPON_PITCH_G_LOAD_MIN = -60.0;
   constexpr double cDEFAULT_WEAPON_YAW_G_LOAD_MAX   =  60.0;

   const QString cKEY_PITCH_G_LOAD_MAX{"pitch_g_load_max"};
   const QString cKEY_PITCH_G_LOAD_MIN{"pitch_g_load_min"};
   const QString cKEY_YAW_G_LOAD_MAX{"yaw_g_load_max"};
   const QString cKEY_LIMITS_GROUP{"g_load_limits - "};
}

G_LimitSettings::G_LimitSettings(const QString& aName)
   : mPitchG_LoadMin{cDEFAULT_AIRCRAFT_PITCH_G_LOAD_MIN}
   , mPitchG_LoadMax{cDEFAULT_AIRCRAFT_PITCH_G_LOAD_MAX}
   , mYawG_LoadMax{cDEFAULT_AIRCRAFT_YAW_G_LOAD_MAX}
   , mName{aName}
   , mIsAircraft{true}
{
}

void G_LimitSettings::LoadFromFile(const QJsonObject& aFile)
{
   auto typeIter = aFile.constFind("VehicleType");
   if (typeIter != aFile.constEnd())
   {
      if (typeIter->toString() == "Aircraft")
      {
         mIsAircraft = true;
      }
      else if (typeIter->toString() == "Weapon")
      {
         mIsAircraft = false;
      }
      else
      {
         throw std::runtime_error("Unknown Vehicle Type");
      }
   }

   auto groupIter = aFile.constFind(GroupKey(mName));
   if (groupIter != aFile.constEnd())
   {
      QJsonObject limits = groupIter->toObject();

      auto pitchMin = limits.constFind(cKEY_PITCH_G_LOAD_MIN);
      if (pitchMin != limits.constEnd())
      {
         mPitchG_LoadMin = pitchMin->toDouble(DefaultMinPitchG_Load());
      }

      auto pitchMax = limits.constFind(cKEY_PITCH_G_LOAD_MAX);
      if (pitchMax != limits.constEnd())
      {
         mPitchG_LoadMax = pitchMax->toDouble(DefaultMaxPitchG_Load());
      }

      auto yawMax = limits.constFind(cKEY_YAW_G_LOAD_MAX);
      if (yawMax != limits.constEnd())
      {
         mYawG_LoadMax = yawMax->toDouble(DefaultMaxYawG_Load());
      }
   }
   else
   {
      mPitchG_LoadMin = DefaultMinPitchG_Load();
      mPitchG_LoadMax = DefaultMaxPitchG_Load();
      mYawG_LoadMax = DefaultMaxYawG_Load();
   }
}

void G_LimitSettings::SaveToFile(QJsonObject& aFile) const
{
   QJsonObject limitsObject{};
   limitsObject.insert(cKEY_PITCH_G_LOAD_MAX, QJsonValue{mPitchG_LoadMax});
   limitsObject.insert(cKEY_PITCH_G_LOAD_MIN, QJsonValue{mPitchG_LoadMin});
   limitsObject.insert(cKEY_YAW_G_LOAD_MAX, QJsonValue{mYawG_LoadMax});
   aFile[GroupKey(mName)] = limitsObject;
}

bool G_LimitSettings::LimitsAreValid() const
{
   return mPitchG_LoadMin < mPitchG_LoadMax && mYawG_LoadMax > 0.0;
}

QString G_LimitSettings::GroupKey(const QString& aName)
{
   return cKEY_LIMITS_GROUP + aName;
}

QString G_LimitSettings::PilotsGuidanceName()
{
   return QString("Pilots/Guidance");
}

QString G_LimitSettings::HardwareAutopilotName()
{
   return QString("Hardware Autopilot");
}

double G_LimitSettings::DefaultMaxPitchG_Load() const
{
   if (mIsAircraft)
   {
      return cDEFAULT_AIRCRAFT_PITCH_G_LOAD_MAX;
   }
   else
   {
      return cDEFAULT_WEAPON_PITCH_G_LOAD_MAX;
   }
}

double G_LimitSettings::DefaultMinPitchG_Load() const
{
   if (mIsAircraft)
   {
      return cDEFAULT_AIRCRAFT_PITCH_G_LOAD_MIN;
   }
   else
   {
      return cDEFAULT_WEAPON_PITCH_G_LOAD_MIN;
   }
}

double G_LimitSettings::DefaultMaxYawG_Load() const
{
   if (mIsAircraft)
   {
      return cDEFAULT_AIRCRAFT_YAW_G_LOAD_MAX;
   }
   else
   {
      return cDEFAULT_WEAPON_YAW_G_LOAD_MAX;
   }
}
