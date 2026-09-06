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

#ifndef WSFANNOTATIONDEF_HPP
#define WSFANNOTATIONDEF_HPP

#include "UtColor.hpp"
#include "UtUnitTypes.hpp"

namespace AnnotationData
{
// Enum is used over enum class for compatibility with event pipe message auto-generated code
enum class PresetType
{
   cONE,
   cTWO,
   cTHREE,
   cFOUR,
   cHEART,
   cDIAMOND,
   cSPADE,
   cCLUB
};
enum class AnnotationType
{
   cTEXT,
   cPRESET,
   cIMAGE
};

struct DecorationData
{
   std::string    mEntityName;
   AnnotationType mType = AnnotationType::cTEXT;
   std::string    mInfo;
};

struct PoiData
{
   std::string mPoiName;
   double      mLat{0};
   double      mLon{0};
   double      mAlt{0};
   bool        mIsBullseye{false};
};

struct RangeRingData
{
   std::string mRingName;
   std::string mReferenceName; // can be either a platform or Poi
   double      mLat{0};
   double      mLon{0};

   bool    mShowRangeLabels{false};
   UtColor mRangeLabelColor{1, 1, 1};

   UtColor mRingColor{1, 1, 1};
   int     mRingWidth{1};
   double  mCenterRadius{111120};
   int     mAdditionalRings{0};
   double  mAdditionalRadii{18520};

   bool    mShowAngleLabels{false};
   UtColor mAngleLabelColor{1, 1, 1};
   UtColor mRadialColor{1, 1, 1};
   int     mRadialWidth{1};
   double  mCenterAngle{UtUnitAngle::ConvertToStandard(0, UtUnitAngle::cDEGREES)};
   double  mAngleWidth{UtUnitAngle::ConvertToStandard(360, UtUnitAngle::cDEGREES)};
   double  mRadialRate{UtUnitAngle::ConvertToStandard(0, UtUnitAngle::cDEGREES)};
   bool    mAlignPlatform{true};
};

struct AnnotationInfo
{
   std::vector<DecorationData> mDecorations;
   std::vector<PoiData>        mPois;
   std::vector<RangeRingData>  mRangeRings;
};
} // namespace AnnotationData


#endif
