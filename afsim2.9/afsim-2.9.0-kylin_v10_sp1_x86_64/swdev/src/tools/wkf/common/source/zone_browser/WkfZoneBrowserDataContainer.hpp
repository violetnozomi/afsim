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
#ifndef WkDataContainerZoneBrowser_HPP
#define WkDataContainerZoneBrowser_HPP

#include "wkf_common_export.h"

#include <memory>

#include <QObject>

#include "UtColor.hpp"
#include "UtMemory.hpp"
#include "UtOptional.hpp"

namespace wkf
{
extern const std::string cGLOBAL_NAME;

enum class ZoneType
{
   cCIRCULAR,
   cELLIPTICAL,
   cSPHERICAL,
   cPOLYGONAL
};

struct ZonePointStruct
{
   ZonePointStruct(const double& aX, const double& aY)
      : mX(aX)
      , mY(aY)
   {
   }
   double mX;
   double mY;
};

struct CommonZoneVariables
{
   using AuxDataMap = std::map<std::string, std::string>;

   std::string mSourceZoneName;
   std::string mZoneName;
   double      mMaxAltitude;
   bool        mMaxAltitudeSet;
   double      mMinAltitude;
   bool        mMinAltitudeSet;
   std::string mReferencePlatformName;
   double      mReferenceLat;
   double      mReferenceLon;
   double      mHeading;
   bool        mLocationDefined; // if true, the Zone is fixed at some location or relative to a reference platform.
   bool        mPreview = false; // Only used for drawing purposes.
   AuxDataMap  mAuxData;
};

struct CircularZoneVariables
{
   double mRadius;
   double mMinRadius;
   double mStartAngle;
   double mStopAngle;
};

struct EllipticalZoneVariables
{
   double mLatAxis;
   double mLonAxis;
   double mMinRadius;
   double mStartAngle;
   double mStopAngle;
};

struct SphericalZoneVariables
{
   double mRadius;
   double mMinRadius;
   double mStartAngle;
   double mStopAngle;
};

struct PolygonalZoneVariables
{
   bool                         mUseLatLon;
   std::vector<ZonePointStruct> mPoints;
};


// using shared_ptr means that the virtual base destructor is not necessary
class WKF_COMMON_EXPORT ZoneData
{
public:
   ZoneData(const CommonZoneVariables& aCommonData)
      : mCommonData(aCommonData)
   {
   }
   virtual ~ZoneData() = default;

   virtual ZoneType                  GetZoneType() const = 0;
   virtual std::unique_ptr<ZoneData> Clone() const       = 0;

   const std::string& GetZoneName() const { return mCommonData.mZoneName; }
   const std::string& GetReferencePlatformName() const { return mCommonData.mReferencePlatformName; }
   const std::string& GetSourceZone() const { return mCommonData.mSourceZoneName; }
   const CommonZoneVariables::AuxDataMap& GetAuxData() const { return mCommonData.mAuxData; }
   double                                 GetReferenceLat() const { return mCommonData.mReferenceLat; }
   double                                 GetReferenceLon() const { return mCommonData.mReferenceLon; }
   double                                 GetMinAlt() const { return mCommonData.mMinAltitude; }
   double                                 GetMaxAlt() const { return mCommonData.mMaxAltitude; }
   double                                 GetHeading() const { return mCommonData.mHeading; }
   bool                                   GetLocationDefined() const { return mCommonData.mLocationDefined; }
   bool                                   IsMinAltSet() const { return mCommonData.mMinAltitudeSet; }
   bool                                   IsMaxAltSet() const { return mCommonData.mMaxAltitudeSet; }
   bool                                   IsPreview() const { return mCommonData.mPreview; }

protected:
   CommonZoneVariables mCommonData;
};

class WKF_COMMON_EXPORT CircularZoneData : public ZoneData
{
public:
   CircularZoneData(const CommonZoneVariables& aCommonData, const CircularZoneVariables& aCircularData)
      : ZoneData(aCommonData)
      , mCircularZoneData(aCircularData)
   {
   }

   CircularZoneData(const CircularZoneData& aSrc)
      : ZoneData(aSrc.mCommonData)
      , mCircularZoneData(aSrc.mCircularZoneData)
   {
   }

   ZoneType                          GetZoneType() const override { return ZoneType::cCIRCULAR; }
   double                            GetRadius() const { return mCircularZoneData.mRadius; }
   double                            GetMinRadius() const { return mCircularZoneData.mMinRadius; }
   double                            GetStartAngle() const { return mCircularZoneData.mStartAngle; }
   double                            GetStopAngle() const { return mCircularZoneData.mStopAngle; }
   virtual std::unique_ptr<ZoneData> Clone() const override { return ut::make_unique<CircularZoneData>(*this); }

private:
   CircularZoneVariables mCircularZoneData;
};

class WKF_COMMON_EXPORT EllipticalZoneData : public ZoneData
{
public:
   EllipticalZoneData(const CommonZoneVariables& aCommonData, const EllipticalZoneVariables& aEllipticalZoneData)
      : ZoneData(aCommonData)
      , mEllipticalZoneData(aEllipticalZoneData)
   {
   }

   EllipticalZoneData(const EllipticalZoneData& aSrc)
      : ZoneData(aSrc.mCommonData)
      , mEllipticalZoneData(aSrc.mEllipticalZoneData)
   {
   }

   ZoneType GetZoneType() const override { return ZoneType::cELLIPTICAL; }
   double   GetLatAxis() const { return mEllipticalZoneData.mLatAxis; }
   double   GetLonAxis() const { return mEllipticalZoneData.mLonAxis; }
   double   GetMinRadius() const { return mEllipticalZoneData.mMinRadius; }
   double   GetStartAngle() const { return mEllipticalZoneData.mStartAngle; }
   double   GetStopAngle() const { return mEllipticalZoneData.mStopAngle; }

   std::unique_ptr<ZoneData> Clone() const override { return ut::make_unique<EllipticalZoneData>(*this); }

private:
   EllipticalZoneVariables mEllipticalZoneData;
};

class WKF_COMMON_EXPORT SphericalZoneData : public ZoneData
{
public:
   SphericalZoneData(const CommonZoneVariables& aCommonData, const SphericalZoneVariables& aSphericalZoneData)
      : ZoneData(aCommonData)
      , mSphericalZoneData(aSphericalZoneData)
   {
   }

   SphericalZoneData(const SphericalZoneData& aSrc)
      : ZoneData(aSrc.mCommonData)
      , mSphericalZoneData(aSrc.mSphericalZoneData)
   {
   }

   ZoneType                  GetZoneType() const override { return ZoneType::cSPHERICAL; }
   double                    GetRadius() const { return mSphericalZoneData.mRadius; }
   double                    GetMinRadius() const { return mSphericalZoneData.mMinRadius; }
   double                    GetStartAngle() const { return mSphericalZoneData.mStartAngle; }
   double                    GetStopAngle() const { return mSphericalZoneData.mStopAngle; }
   std::unique_ptr<ZoneData> Clone() const override { return ut::make_unique<SphericalZoneData>(*this); }

private:
   SphericalZoneVariables mSphericalZoneData;
};

class WKF_COMMON_EXPORT PolygonalZoneData : public ZoneData
{
public:
   PolygonalZoneData(const CommonZoneVariables& aCommonData, const PolygonalZoneVariables& aPolygonalZoneData)
      : ZoneData(aCommonData)
      , mPolygonalZoneData(aPolygonalZoneData)
   {
   }

   PolygonalZoneData(const PolygonalZoneData& aSrc)
      : ZoneData(aSrc.mCommonData)
      , mPolygonalZoneData(aSrc.mPolygonalZoneData)
   {
   }

   ZoneType                            GetZoneType() const override { return ZoneType::cPOLYGONAL; }
   const std::vector<ZonePointStruct>& GetZonePoints() const { return mPolygonalZoneData.mPoints; }
   bool                                UseLatLon() const { return mPolygonalZoneData.mUseLatLon; }

   std::unique_ptr<ZoneData> Clone() const override { return ut::make_unique<PolygonalZoneData>(*this); }

private:
   PolygonalZoneVariables mPolygonalZoneData;
};

class WKF_COMMON_EXPORT ZoneSetData
{
public:
   using ZoneSet    = std::vector<std::unique_ptr<ZoneData>>;
   using AuxDataMap = std::map<std::string, std::string>;

   ZoneSetData() = delete;
   ZoneSetData(const std::string& aSetName, const std::string& aPlatformName)
      : mSetName(aSetName)
      , mPlatformName(aPlatformName)
   {
      if (mPlatformName.empty())
      {
         mPlatformName = cGLOBAL_NAME;
      }
   }
   ZoneSetData(const ZoneSetData& aSource);
   ZoneSetData& operator=(const ZoneSetData& aSource);

   ~ZoneSetData() = default;

   const std::string&    GetSetName() const { return mSetName; }
   const std::string&    GetPlatformName() const { return mPlatformName; }
   const ZoneSet&        GetZones() const { return mZones; }
   ZoneSet&              GetZones() { return mZones; }
   const AuxDataMap&     GetAuxData() const { return mAuxData; }
   ut::optional<UtColor> GetFillColor() const { return mFillColor; }
   ut::optional<UtColor> GetLineColor() const { return mLineColor; }

   void SetFillColor(const ut::optional<UtColor>& aColor) { mFillColor = aColor; }
   void SetLineColor(const ut::optional<UtColor>& aColor) { mLineColor = aColor; }

   void AddCircularData(CommonZoneVariables& aCommonData, const CircularZoneVariables& aCircularData);
   void AddEllipticalData(CommonZoneVariables& aCommonData, const EllipticalZoneVariables& aEllipticalData);
   void AddSphericalData(CommonZoneVariables& aCommonData, const SphericalZoneVariables& aSphericalData);
   void AddPolygonalData(CommonZoneVariables& aCommonData, const PolygonalZoneVariables& aPolygonalData);

private:
   std::string GetUniqueName() { return mPlatformName + "_" + mSetName + "_" + std::to_string(++mIdManager); }

   std::string           mSetName;
   std::string           mPlatformName;
   ZoneSet               mZones;
   AuxDataMap            mAuxData;
   size_t                mIdManager{0};
   ut::optional<UtColor> mFillColor;
   ut::optional<UtColor> mLineColor;
};

class WKF_COMMON_EXPORT ZoneBrowserDataContainer : public QObject
{
   Q_OBJECT
public:
   ZoneBrowserDataContainer()
      : QObject()
      , mPreviewZonePtr(nullptr)
   {
   }

   ~ZoneBrowserDataContainer() override                      = default;
   ZoneBrowserDataContainer(const ZoneBrowserDataContainer&) = delete;
   ZoneBrowserDataContainer& operator=(const ZoneBrowserDataContainer&) = delete;

   void ClearData();
   bool IsEmpty() const { return mGlobalZones.empty() && mZoneMap.empty() && mPreviewZonePtr == nullptr; }
   void NotifyWidget() { emit DataCopyComplete(); }

   void AddZoneSet(const ZoneSetData& aZoneSet, bool aShow = false);

   void UpdateData(const ZoneSetData& aZoneSet);
   void UpdateColor(const ZoneSetData& aZoneSet);

   void UpdatePreview(CommonZoneVariables          aCommonData,
                      const CircularZoneVariables& aCircularData,
                      const std::string&           aSetName,
                      const std::string&           aPlatformName);
   void UpdatePreview(CommonZoneVariables            aCommonData,
                      const EllipticalZoneVariables& aEllipticalData,
                      const std::string&             aSetName,
                      const std::string&             aPlatformName);
   void UpdatePreview(CommonZoneVariables           aCommonData,
                      const SphericalZoneVariables& aSphericalData,
                      const std::string&            aSetName,
                      const std::string&            aPlatformName);
   void UpdatePreview(CommonZoneVariables           aCommonData,
                      const PolygonalZoneVariables& aPolygonalData,
                      const std::string&            aSetName,
                      const std::string&            aPlatformName);

   void RemovePreview();

   void RemoveData(const std::string& aZoneName, const std::string& aPlatformName);
   void RemovePlatform(const std::string& aPlatformName);

   ZoneSetData* GetGlobalZone(const std::string& aZoneName) const;
   ZoneSetData* GetLocalZone(const std::string& aPlatformName, const std::string& aZoneName) const;
   ZoneSetData* GetPreviewZone() const { return mPreviewZonePtr.get(); }

signals:
   void ClearZoneData();
   void DataCopyComplete();

   void ZoneAdded(ZoneSetData* aDataPtr, bool aShow);
   void ZoneUpdated(const std::string& aZoneName, const std::string& aPlatformName);
   void ZoneColorUpdated(const std::string& aZoneName, const std::string& aPlatformName);
   void ZoneDeleted(ZoneSetData* aDataPtr);
   void PreviewUpdated();
   void PreviewPendingDeletion();

   void PlatformRemoved(const std::string& aPlatformName);

private:
   std::unique_ptr<ZoneSetData>                                     mPreviewZonePtr;
   std::vector<std::unique_ptr<ZoneSetData>>                        mGlobalZones;
   std::map<std::string, std::vector<std::unique_ptr<ZoneSetData>>> mZoneMap;
};
} // namespace wkf

#endif
