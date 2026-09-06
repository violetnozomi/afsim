// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VAOVERLAYWDB_HPP
#define VAOVERLAYWDB_HPP

#include "vespatk_export.h"

#include "VaCallbackHolder.hpp"
#include "VaOverlayGl.hpp"

class UtoMapProjection;

#define MIN_WDB_LVL 1
#define MAX_WDB_LVL 5

namespace vespa
{

class VESPATK_EXPORT VaOverlayWdb : public VaOverlayGl
{
public:
   VaOverlayWdb();

   ~VaOverlayWdb() override = default;

   VaOverlayWdb(const std::string& aName);

   VaOverlayWdb* Clone() const override;

   // Hey, color and width don't really work, yet.
   // WdbMap will take the input, but won't reload resident data
   // so it will only take effect for things that are not already
   // loaded.
   virtual void SetColor(const std::string& aLayerId, const float* aColor);
   virtual void SetWidth(const std::string& aLayerId, const float& aWidth);

   virtual void ShowLayer(const std::string& aLayerId);
   virtual void HideLayer(const std::string& aLayerId);

   void SetUrl(const std::string& aUrl) { mURL = aUrl; }

   void MapProjectionChangedCB(unsigned int aViewer, const UtoMapProjection* aProjection);

private:
   enum
   {
      MtContinents      = 0x01,
      MtIslands         = 0x02,
      MtCountryBorders  = 0x04,
      MtInternalBorders = 0x08,
      MtLakes           = 0x10,
      MtRivers          = 0x20,
      MtAll             = 0x3F
   };
   struct WdbPnt
   {
   public:
      WdbPnt() = default;
      WdbPnt(float aLat, float aLon)
         : mLat(aLat)
         , mLon(aLon)
      {
      }

      float mLat;
      float mLon;
   };

   struct WdbSeg
   {
   public:
      unsigned int       mIndex;
      unsigned int       mCount;
      unsigned short int mType;
      unsigned short int mColor;
      GLuint             mDisplayList;
      float              mMinLat;
      float              mMaxLat;
      float              mMinLon;
      float              mMaxLon;
   };

   void DrawFunction() override;

   void GenerateWdbData();
   void ResetWdbData();
   void DrawWdbData(VaCamera* aCameraPtr);
   void ReadMap(unsigned int aMapLevel);
   void DrawMap(VaCamera* aCameraPtr);
   void MakeDisplayList(VaCamera* aCameraPtr, WdbSeg& aSegment);

   VaCallbackHolder mCallbacks;

   std::string mURL;

   std::map<unsigned int, float>    mWidthMap;
   std::map<unsigned int, float[3]> mColorMap;

   unsigned int mActiveLayers;
   bool         mGeocentric;
   bool         mWdbDataGenerated;

   struct WdbFilePnt
   {
      short code;
      short lat;
      short lon;
   };

   std::vector<WdbSeg> mSegs;
   std::vector<WdbPnt> mPnts;
   unsigned int        mMapLevel;
   unsigned int        mSegIndex[MAX_WDB_LVL + 1];
   unsigned int        mSegCount[MAX_WDB_LVL + 1];
   unsigned int        mPntCount[MAX_WDB_LVL + 1];
};

} // namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayWdb)
#endif
