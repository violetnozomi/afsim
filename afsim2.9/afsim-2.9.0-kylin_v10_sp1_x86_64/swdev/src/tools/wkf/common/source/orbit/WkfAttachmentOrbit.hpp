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
#ifndef WKFATTACHMENTORBIT_HPP
#define WKFATTACHMENTORBIT_HPP

#include "wkf_common_export.h"

#include "UtOptional.hpp"
#include "UtoMapProjection.hpp"
#include "VaAttachment.hpp"

namespace osg
{
class Program;
}

namespace wkf
{
class WKF_COMMON_EXPORT AttachmentOrbit : public vespa::VaAttachment
{
public:
   AttachmentOrbit(vespa::VaEntity& aEntityPtr, vespa::VaViewer* aViewerPtr, bool aCelestial = false);
   ~AttachmentOrbit() override;

   void Clear();
   void Add(double aSimTime,
            double aSemiMajor,
            double aEccentricity,
            double aRAAN,
            double aInclination,
            double aArgumentOfPeriapsis,
            double aTrueAnomaly,
            bool   aManeuver);
   void UpdateTimeAngle(double aTimeAng, double aSimTime);

   void    SetFadeout(int aFadeout);
   void    SetLineWidth(int aLinewidth);
   void    SetColor(const UtColor& aColor);
   bool    ColorIsDefined() const { return mDefinedColor.has_value(); }
   UtColor GetDefinedColor() const { return mDefinedColor.value_or(UtColor(1, 1, 1)); }
   void    SetDefinedColor(const UtColor& aColor) { mDefinedColor = aColor; }
   void    SetPeriods(unsigned int aPeriods);

private:
   void HandleCelestialCamera(bool aShow);

   void ECI_ModeChanged(bool aState);
   void MapProjectionChanged(unsigned int aViewerIndex, const UtoMapProjection* aProjection);

   struct OrbitData
   {
      double mSimTime{0.0};
      double mSemiMajor{1000000.0};
      double mEccentricity{0.0};
      double mRAAN{0.0};
      double mInclination{0.0};
      double mArgumentOfPeriapsis{0.0};
      double mTrueAnomaly{0.0};
      bool   mManeuver{false};
      double mFadeout{0.0};
   };
   using ShapeData = std::pair<OrbitData, UtoShape*>;

   void Rebuild(const std::string& aName, ShapeData& aShapeData);

   ShapeData                        mActiveShape;
   std::string                      mActiveShapeName;
   std::map<std::string, ShapeData> mOldShapes;
   static osg::Program*             sOsgProgram;
   int                              mFadeout;
   int                              mLineWidth;
   UtColor                          mColor;
   ut::optional<UtColor>            mDefinedColor;
   bool                             mCelestial;
   unsigned int                     mPeriods{1};
};
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::AttachmentOrbit)

#endif
