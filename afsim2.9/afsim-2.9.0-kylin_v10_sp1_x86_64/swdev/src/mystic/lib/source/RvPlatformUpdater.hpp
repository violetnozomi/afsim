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
#ifndef RvPlatformUpdater_HPP
#define RvPlatformUpdater_HPP

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "WkfUnitUpdaters.hpp"

namespace rv
{
template<class BASE_UPDATER>
class PlatformUpdaterT : public BASE_UPDATER, public QObject
{
   static_assert(std::is_base_of<wkf::Updater, BASE_UPDATER>::value, "Template type must be subclass of wkf::Updater");

   using BaseUpdater = BASE_UPDATER;

public:
   PlatformUpdaterT(const QString& aUpdaterName, const std::string& aPlatformName)
      : BaseUpdater(aUpdaterName, aPlatformName)
      , QObject()
   {
      connect(&rvEnv, &Environment::AdvanceTimeRead, this, &PlatformUpdaterT<BaseUpdater>::TimerRead);
      connect(&rvEnv, &Environment::RegularRead, this, &PlatformUpdaterT<BaseUpdater>::TimerRead);
   }

   ~PlatformUpdaterT() override = default;

   void TimerRead(const ResultData& aResultData)
   {
      ResultDb* db = aResultData.GetDb();
      if (db)
      {
         ResultPlatform* platform = db->FindPlatformByName(BaseUpdater::GetPlatformOfInterest());
         if (platform)
         {
            BaseUpdater::mDataValid = ReadData(platform, aResultData.GetSimTime());
         }
      }
   }

protected:
   virtual bool ReadData(rv::ResultPlatform* aPlatform, double aSimTime) = 0;
};

using PlatformAccelerationUpdater        = PlatformUpdaterT<wkf::AccelerationValueUpdater>;
using PlatformAltitudeUpdater            = PlatformUpdaterT<wkf::AltitudeValueUpdater>;
using PlatformHighAltitudeUpdater        = PlatformUpdaterT<wkf::HighAltitudeValueUpdater>;
using PlatformAngleUpdater               = PlatformUpdaterT<wkf::AngleValueUpdater>;
using PlatformAngularAccelerationUpdater = PlatformUpdaterT<wkf::AngularAccelerationValueUpdater>;
using PlatformAngularRateUpdater         = PlatformUpdaterT<wkf::AngularRateValueUpdater>;
using PlatformForceUpdater               = PlatformUpdaterT<wkf::ForceValueUpdater>;
using PlatformFrequencyUpdater           = PlatformUpdaterT<wkf::FrequencyValueUpdater>;
using PlatformLengthUpdater              = PlatformUpdaterT<wkf::LengthValueUpdater>;
using PlatformMassUpdater                = PlatformUpdaterT<wkf::MassValueUpdater>;
using PlatformPowerUpdater               = PlatformUpdaterT<wkf::PowerValueUpdater>;
using PlatformPowerDBUpdater             = PlatformUpdaterT<wkf::PowerDBValueUpdater>;
using PlatformPressureUpdater            = PlatformUpdaterT<wkf::PressureValueUpdater>;
using PlatformRatioUpdater               = PlatformUpdaterT<wkf::RatioValueUpdater>;
using PlatformSpeedUpdater               = PlatformUpdaterT<wkf::SpeedValueUpdater>;
using PlatformTimeUpdater                = PlatformUpdaterT<wkf::TimeValueUpdater>;
using PlatformUnitlessUpdater            = PlatformUpdaterT<wkf::UnitlessValueUpdater>;
using PlatformUpdater                    = PlatformUpdaterT<wkf::Updater>;
using PlatformLatitudeUpdater            = PlatformUpdaterT<wkf::LatitudeValueUpdater>;
using PlatformLongitudeUpdater           = PlatformUpdaterT<wkf::LongitudeValueUpdater>;
} // namespace rv
#endif
