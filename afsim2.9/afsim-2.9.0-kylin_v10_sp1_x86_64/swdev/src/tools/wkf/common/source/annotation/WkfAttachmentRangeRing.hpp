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

#ifndef WKFATTACHMENTRANGERING_HPP
#define WKFATTACHMENTRANGERING_HPP

#include "wkf_common_export.h"

#include <QColor>
#include <osg/Array>

#include "UtMath.hpp"
#include "UtoMapProjection.hpp"
#include "VaAttachment.hpp"

namespace osg
{
class Geode;
class Geometry;
} // namespace osg

class UtoShape;

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentRangeRing : public vespa::VaAttachment
{
public:
   struct WKF_COMMON_EXPORT Properties final
   {
      QString mName                 = "rangeRing";
      float   mBaseRadius           = 111120.0F; // 60 nmi in meters, the radius of the first ring
      int     mAdditionalRings      = 0;
      float   mAdditionalRingRadius = 18520.0f; // 10 nmi in meters
      float   mCenterAngle          = 0;
      float   mWidth                = static_cast<float>(UtMath::cTWO_PI); // 360 deg
      float   mRadialRate           = 0.0;
      QColor  mRingColor            = Qt::white;
      QColor  mRadialColor          = Qt::white;
      int     mRingLineWidth        = 1;
      int     mRadialLineWidth      = 1;
      bool    mShowRangeLabels      = false;
      bool    mShowAngleLabels      = false;
      bool    mAlignWithPlatform    = true;
      QColor  mRangeLabelColor      = Qt::white;
      QColor  mAngleLabelColor      = Qt::white;
      bool    mExportable           = true;
   };

   AttachmentRangeRing(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr, const std::string& aName);

   void  SetCenterRadius(float aValue);
   float GetCenterRadius() const { return mProperties.mBaseRadius; }

   void SetAdditionalRings(int aNumber, float aValue);

   float GetAdditionalRadii() const { return mProperties.mAdditionalRingRadius; }
   int   GetNumberAdditionalRings() const { return mProperties.mAdditionalRings; }

   void  SetCenterAngle(float aValue);
   float GetCenterAngle() const { return mProperties.mCenterAngle; }

   void  SetAngleWidth(float aValue);
   float GetAngleWidth() const { return mProperties.mWidth; }

   void  SetRadialRate(float aValue);
   float GetRadialRate() const { return mProperties.mRadialRate; }

   void          SetRingColor(const QColor& aColor);
   const QColor& GetRingColor() const { return mProperties.mRingColor; }

   void SetRingLineWidth(int aWidth);
   int  GetRingLineWidth() { return mProperties.mRingLineWidth; }

   void          SetRadialColor(const QColor& aColor);
   const QColor& GetRadialColor() const { return mProperties.mRadialColor; }

   void SetRadialLineWidth(int aWidth);
   int  GetRadialLineWidth() const { return mProperties.mRadialLineWidth; }

   void SetRangeLabelsEnabled(bool aState);
   bool GetRangeLabelsEnabled() const { return mProperties.mShowRangeLabels; }

   void SetAngleLabelsEnabled(bool aState);
   bool GetAngleLabelsEnabled() const { return mProperties.mShowAngleLabels; }

   void SetAlignWithPlatformHeading(bool aState);
   bool GetAlignWithPlatformHeading() const { return mProperties.mAlignWithPlatform; }

   void          SetRangeLabelColor(const QColor& aColor);
   const QColor& GetRangeLabelColor() const { return mProperties.mRangeLabelColor; }

   void   SetAngleLabelColor(const QColor& aColor);
   QColor GetAngleLabelColor() const { return mProperties.mAngleLabelColor; }

   const Properties& GetProperties() const { return mProperties; }
   void              SetProperties(const Properties& aProps);

   void SetName(const std::string& aName) override;

private:
   void Build();

   double GetHeading() const;

   void         DrawRing(osg::Geode* aGeodePtr, float aRadius);
   unsigned int DrawRadial(float aAngle, osg::Vec3Array* aVectorArray, osg::Geometry* aGeometry, unsigned int aStart);
   void         MapProjectionChangedCB(unsigned int aViewerId, const UtoMapProjection* aProjection);
   void         DrawRangeLabel(float aRange);
   void         DrawAngleLabel(float aRange, float aAngle);
   void         UnitsChangedCB(int aUnitType, int aUnit);
   void         ConvertToLocalFlat(osg::Vec3& aXYZ) const;
   void         EntityMovedCB(vespa::VaEntity* aEntityPtr, bool aMoving);

   UtoShape*        mShapePtr;
   Properties       mProperties;
   UtoMapProjection mProjection;
   double           mHeading;
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentRangeRing)

#endif
