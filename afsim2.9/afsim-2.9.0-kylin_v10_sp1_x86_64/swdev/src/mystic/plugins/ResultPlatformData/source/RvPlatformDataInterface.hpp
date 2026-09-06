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
#ifndef RVPLATFORMDATAINTERFACE_HPP
#define RVPLATFORMDATAINTERFACE_HPP

#include <set>

#include <QMutex>
#include <QObject>

#include "RvPlatformDataUpdater.hpp"

namespace rv
{
class ResultData;
}

namespace RvPlatformData
{
class Interface : public QObject
{
   Q_OBJECT

public:
   Interface(QObject* parent);
   ~Interface() override = default;

   void SetPlatformOfInterest(const std::string& aPlatformName);

   void AdvanceTimeRead(const rv::ResultData& aData);

   struct PlatformData
   {
      QString name;

      QString type;
      QString side;
      QString icon;
      QString spatial_domain;
      QString status;
      double  startTime;
      double  endTime;

      double                                                   damageFactor;
      double                                                   fuelCapacity;
      bool                                                     fuelCapacityValid;
      double                                                   fuelCurrent;
      double                                                   latitude;
      double                                                   longitude;
      double                                                   altitude_m;
      double                                                   x;
      double                                                   y;
      double                                                   z;
      double                                                   yaw;
      double                                                   pitch;
      double                                                   roll;
      double                                                   psi;
      double                                                   theta;
      double                                                   phi;
      double                                                   speed_m;
      double                                                   mach;
      bool                                                     machValid;
      double                                                   vel_n;
      double                                                   vel_e;
      double                                                   vel_d;
      double                                                   vel_x;
      double                                                   vel_y;
      double                                                   vel_z;
      double                                                   accel_m;
      double                                                   accel_n;
      double                                                   accel_e;
      double                                                   accel_d;
      double                                                   accel_x;
      double                                                   accel_y;
      double                                                   accel_z;
      bool                                                     state_valid;
      std::set<QString>                                        categories;
      std::map<QString, std::set<std::pair<QString, QString>>> parts;
   };

   PlatformData GetPlatformData();

private:
   enum
   {
      LOOKUP_REQUIRED = 0,
      SCENARIO        = -1
   };
   mutable QMutex mMutex;
   PlatformData   mPlatformData;
   int            mIndex;

   std::unique_ptr<wkf::Updater> mSideUpdater;
   std::unique_ptr<wkf::Updater> mTypeUpdater;
   std::unique_ptr<wkf::Updater> mIconUpdater;
   std::unique_ptr<wkf::Updater> mDomainUpdater;
   std::unique_ptr<wkf::Updater> mStatusUpdater;
};
} // namespace RvPlatformData
#endif // RVPLATFORMDATAINTERFACE_HPP
