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

#ifndef WKF_EVENT_MARKER_HPP
#define WKF_EVENT_MARKER_HPP

#include "wkf_common_export.h"

#include <QObject>

#include "WkfEntity.hpp"

namespace wkf
{
struct WKF_COMMON_EXPORT PlatformEventInfo
{
   std::string mPlatform;
};

struct WKF_COMMON_EXPORT WeaponEventInfo
{
   std::string mFiringName;
   std::string mWeaponName;
   std::string mTargetName;

   std::string mResult;

   double mInterceptPk  = 0;
   double mPkDrawn      = 0;
   double mMissDistance = 0;

   double mWeaponSpeed = 0;
   double mTargetSpeed = 0;
};

class WKF_COMMON_EXPORT BaseEventMarker : public QObject, public wkf::Entity
{
   Q_OBJECT
public:
   BaseEventMarker(const std::string& aName, double aTime, const std::string& aType);

   double      GetEventTime() const noexcept { return mTime; }
   std::string GetEventType() const noexcept { return mType; }

protected:
   bool GetNamedEventInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const;

private:
   double      mTime;
   std::string mType;
};

class WKF_COMMON_EXPORT PlatformEventMarker final : public BaseEventMarker
{
public:
   PlatformEventMarker(double aTime, const std::string& aType, const PlatformEventInfo& aInfo);

   bool GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const override;

private:
   std::string GetSummary() const;

   PlatformEventInfo mInfo;
};

class WKF_COMMON_EXPORT WeaponEventMarker final : public BaseEventMarker
{
public:
   WeaponEventMarker(double aTime, const std::string& aType, const WeaponEventInfo& aInfo);

   bool GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const override;

private:
   std::string GetSummary() const;

   WeaponEventInfo mInfo;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::BaseEventMarker);
VA_DECLARE_OBJECT_TYPE(wkf::PlatformEventMarker);
VA_DECLARE_OBJECT_TYPE(wkf::WeaponEventMarker);

#endif
