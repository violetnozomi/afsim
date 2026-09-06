// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RELATIVEGEOMETRYUPDATER_HPP
#define RELATIVEGEOMETRYUPDATER_HPP

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "WkfUnitUpdaters.hpp"
#include "WkfUpdater.hpp"

namespace RelativeGeometry
{
template<class BASE_UPDATER>
class DualPlatformUpdaterT : public BASE_UPDATER, public QObject
{
   static_assert(std::is_base_of<wkf::Updater, BASE_UPDATER>::value, "Template type must be subclass of wkf::Updater");

   using BaseUpdater = BASE_UPDATER;

public:
   DualPlatformUpdaterT(const QString& aUpdaterName, const QString& aFromPlatformName, const QString& aToPlatformName)
      : BaseUpdater(aUpdaterName, "")
      , QObject()
      , mFromPlatformName(aFromPlatformName)
      , mToPlatformName(aToPlatformName)
   {
      connect(&rvEnv, &rv::Environment::AdvanceTimeRead, this, &DualPlatformUpdaterT<BaseUpdater>::TimerRead);
      connect(&rvEnv, &rv::Environment::RegularRead, this, &DualPlatformUpdaterT<BaseUpdater>::TimerRead);
   }

   ~DualPlatformUpdaterT() override = default;

   void TimerRead(const rv::ResultData& aResultData)
   {
      if (mFromPlatformName == mToPlatformName)
      {
         BaseUpdater::mDataValid = false;
         return;
      }
      rv::ResultDb* db = aResultData.GetDb();
      if (db)
      {
         rv::ResultPlatform* fromPlatform = db->FindPlatformByName(mFromPlatformName);
         rv::ResultPlatform* toPlatform   = db->FindPlatformByName(mToPlatformName);
         if (fromPlatform && toPlatform && (fromPlatform != toPlatform))
         {
            BaseUpdater::mDataValid = ReadData(fromPlatform, toPlatform, aResultData.GetSimTime());
         }
      }
   }

   QString GetValueString() const override
   {
      if (BaseUpdater::mDataValid)
      {
         return BaseUpdater::GetValueString();
      }
      else
      {
         return "INVALID";
      }
   }

protected:
   virtual bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) = 0;
   QString      mFromPlatformName;
   QString      mToPlatformName;
};

using DualPlatformLengthUpdater = DualPlatformUpdaterT<wkf::LengthValueUpdater>;
using DualPlatformAngleUpdater  = DualPlatformUpdaterT<wkf::AngleValueUpdater>;
using DualPlatformSpeedUpdater  = DualPlatformUpdaterT<wkf::SpeedValueUpdater>;

class LinearRangeUpdater : public DualPlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Linear Range";
   LinearRangeUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformLengthUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~LinearRangeUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class RadialUpdater : public DualPlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Radial";
   RadialUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformLengthUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~RadialUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class InTrackUpdater : public DualPlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "In Track";
   InTrackUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformLengthUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~InTrackUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class CrossTrackUpdater : public DualPlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Cross Track";
   CrossTrackUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformLengthUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~CrossTrackUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class BearingUpdater : public DualPlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Bearing";
   BearingUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformAngleUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~BearingUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class ElevationUpdater : public DualPlatformAngleUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Elevation";
   ElevationUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformAngleUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~ElevationUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class DownRangeUpdater : public DualPlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Down Range";
   DownRangeUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformLengthUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~DownRangeUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class CrossRangeUpdater : public DualPlatformLengthUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Cross Range";
   CrossRangeUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformLengthUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~CrossRangeUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
class RangeRateUpdater : public DualPlatformSpeedUpdater
{
public:
   static constexpr const char* cUPDATER_NAME = "Range Rate";
   RangeRateUpdater(const QString& aFromPlatformName = "", const QString& aToPlatformName = "")
      : DualPlatformSpeedUpdater(cUPDATER_NAME, aFromPlatformName, aToPlatformName)
   {
   }
   ~RangeRateUpdater() override = default;

protected:
   bool ReadData(rv::ResultPlatform* aFromPlatform, rv::ResultPlatform* aToPlatform, double aSimTime) override;
};
} // namespace RelativeGeometry

#endif
