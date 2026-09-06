// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WkfTrack_HPP
#define WkfTrack_HPP

#include "wkf_export.h"

#include <string>

#include <QMetaType>
#include <QString>

#include "WkfDefines.hpp"
#include "WkfEntity.hpp"

namespace wkf
{
class WKF_EXPORT TrackId
{
public:
   TrackId()
      : TrackId({}, 0)
   {
   }

   TrackId(const std::string& aOwningPlatformName, int aLocalTrackId)
      : mOwningPlatformName(aOwningPlatformName)
      , mLocalTrackId(aLocalTrackId)
   {
   }

   TrackId(const QString& aTrackString);

   bool IsNull() const { return (mOwningPlatformName.empty() || mLocalTrackId == 0); }

   const std::string& GetPlatformName() const { return mOwningPlatformName; }
   int                GetLocalTrackId() const { return mLocalTrackId; }

   void SetPlatformName(const std::string& aPlatformName) { mOwningPlatformName = aPlatformName; }
   void SetLocalTrackId(int aTrackId) { mLocalTrackId = aTrackId; }

   bool operator<(const TrackId& aRhs) const
   {
      if (mOwningPlatformName < aRhs.mOwningPlatformName)
      {
         return true;
      }
      else if (mOwningPlatformName > aRhs.mOwningPlatformName)
      {
         return false;
      }
      return (mLocalTrackId < aRhs.mLocalTrackId);
   }

   QString toString() const
   {
      return QString("%1:%2").arg(QString::fromStdString(mOwningPlatformName)).arg(mLocalTrackId);
   }

   operator QString() const { return toString(); }

   operator std::string() const { return toString().toStdString(); }

protected:
   std::string mOwningPlatformName;
   int         mLocalTrackId;
};

class WKF_EXPORT Track : public Entity
{
public:
   Track(const TrackId& aId);

   TrackId GetTrackId() { return mTrackId; }

   void               SetSide(const std::string& aSide);
   const std::string& GetSide() const { return mSide; }

   void               SetTrackType(const std::string& aType) { mTrackType = aType; }
   const std::string& GetTrackType() const { return mTrackType; }

   void               SetIcon(const std::string& aIcon) { mIcon = aIcon; }
   const std::string& GetIcon() const { return mIcon; }

   void          SetSpatialDomain(SpatialDomain aDomain) { mSpatialDomain = aDomain; }
   void          SetSpatialDomain(const std::string& aDomainString);
   SpatialDomain GetSpatialDomain() const { return mSpatialDomain; }
   std::string   GetSpatialDomainAsString() const;

   void               SetTargetPlatform(const std::string& aPlatform) { mTargetPlatform = aPlatform; }
   const std::string& GetTargetPlatform() const { return mTargetPlatform; }

   void SetPositionValid(bool aValid) { mPositionValid = aValid; }
   bool GetPositionValid() const { return mPositionValid; }

   void SetVelocityValid(bool aValid) { mVelocityValid = aValid; }
   bool GetVelocityValid() const { return mVelocityValid; }

   std::string GetDescription() override;

   bool GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const override;

private:
   std::string   mSide;
   std::string   mTrackType;
   std::string   mTargetPlatform;
   std::string   mIcon;
   SpatialDomain mSpatialDomain;
   TrackId       mTrackId;
   bool          mPositionValid;
   bool          mVelocityValid;
};

WKF_EXPORT wkf::SpatialDomain StringToSpatialDomain(const std::string& aString);

WKF_EXPORT std::string SpatialDomainToString(wkf::SpatialDomain aSpatialDomain);
} // namespace wkf

Q_DECLARE_METATYPE(wkf::TrackId);
VA_DECLARE_OBJECT_TYPE(wkf::Track)

#endif // WkfTrack_HPP
