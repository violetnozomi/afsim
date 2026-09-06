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

#include "VaOverlayWdb.hpp"

#include <array>
#include <sstream>

#include <osg/GL>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "UtLog.hpp"
#include "VaCamera.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"

namespace vespa
{

VaOverlayWdb::VaOverlayWdb()
   : VaOverlayGl("wdb", cOVERLAY_WORLD)
   , mActiveLayers(0)
   , mGeocentric(true)
   , mWdbDataGenerated(false)
{
   SetType<VaOverlayWdb>();
   EnableLineSmoothing(true);
   for (int i = 0; i < 6; ++i)
   {
      mWidthMap[i]    = 1.0f;
      mColorMap[i][0] = 1.0f;
      mColorMap[i][1] = 1.0f;
      mColorMap[i][2] = 1.0f;
   }
}


VaOverlayWdb::VaOverlayWdb(const std::string& aName)
   : VaOverlayGl(aName, cOVERLAY_WORLD)
   , mActiveLayers(0)
   , mGeocentric(true)
   , mWdbDataGenerated(false)
{
   SetType<VaOverlayWdb>();
   EnableLineSmoothing(true);
   for (int i = 0; i < 6; ++i)
   {
      mWidthMap[i]    = 1.0f;
      mColorMap[i][0] = 1.0f;
      mColorMap[i][1] = 1.0f;
      mColorMap[i][2] = 1.0f;
   }
}

VaOverlayWdb* VaOverlayWdb::Clone() const
{
   return new VaOverlayWdb(*this);
}

void VaOverlayWdb::DrawFunction()
{
   if ((!mWdbDataGenerated) && (!mURL.empty()))
   {
      GenerateWdbData();
   }

   if (!mGeocentric)
   {
      glPushMatrix();
      glTranslatef(0, 0, 100);
   }
   DrawWdbData(GetViewer()->GetCamera());
   if (!mGeocentric)
   {
      glPopMatrix();
   }
}
void VaOverlayWdb::GenerateWdbData()
{
   if (!mWdbDataGenerated)
   {
      mCallbacks.Add(vespa::VaObserver::MapProjectionChanged.Connect(&VaOverlayWdb::MapProjectionChangedCB,
                                                                     this,
                                                                     GetViewer()->GetUniqueId()));
   }

   for (unsigned int i = 0; i < MAX_WDB_LVL + 1; ++i)
   {
      mSegIndex[i] = 0;
      mSegCount[i] = 0;
   }

   mWdbDataGenerated = true;
}

void VaOverlayWdb::ResetWdbData()
{
   std::vector<WdbSeg>::iterator it;
   for (it = mSegs.begin(); it != mSegs.end(); ++it)
   {
      GLuint dl = it->mDisplayList;
      glDeleteLists(dl, 1);
      it->mDisplayList = 0;
   }
}

void VaOverlayWdb::DrawWdbData(VaCamera* aCameraPtr)
{
   double metersPerPixel = aCameraPtr->GetMetersPerPixel();

   unsigned int newMapLevel = 5;
   if (metersPerPixel < 1000.0)
   {
      newMapLevel = 1;
   }
   else if (metersPerPixel < 2000.0)
   {
      // wdb.2.all is broken - it is missing the islands.  Use level 1 for now
      //    newMapLevel = 2;
      newMapLevel = 1;
   }
   else if (metersPerPixel < 10000.0)
   {
      newMapLevel = 3;
   }
   else if (metersPerPixel < 40000.0)
   {
      newMapLevel = 4;
   }

   if (newMapLevel != mMapLevel)
   {
      ReadMap(newMapLevel);
   }

   DrawMap(aCameraPtr);
}

void VaOverlayWdb::ReadMap(unsigned int aMapLevel)
{
   static int num_pnts[] = {0, 178150, 87400, 26225, 13750, 4375};
   // { 0, 178082,  87354,  26174,  13694,   4325 };      // actual values
   static int num_segs[] = {0, 1280, 930, 1280, 1280, 1280};
   // { 0,   1270,    922,   1270,   1270,   1268 };      // actual values

   assert((aMapLevel >= MIN_WDB_LVL) && (aMapLevel <= MAX_WDB_LVL));
   mMapLevel = aMapLevel;

   if (mSegCount[mMapLevel] != 0)
      return; // already loaded

   std::ostringstream oss;
   oss << mURL << "/wdb." << aMapLevel << ".all";
   FILE* filePtr = fopen(oss.str().c_str(), "rb");
   if (filePtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to open file.";
      out.AddNote() << "File: " << oss.str();
      return;
   }

   std::array<WdbFilePnt, 16384> rbuf;
   WdbSeg                        cur_seg;
   bool                          segment_active = false;
   float                         last_lat       = 0.0F;
   float                         last_lon       = 0.0F;

   // Preallocate space in vectors to improve performance.  The code will
   // still work if the initial estimates are not big enough.

   mPnts.reserve(mPnts.size() + num_pnts[aMapLevel] + 100);
   mSegs.reserve(mSegs.size() + num_segs[aMapLevel] + 100);
   mSegIndex[aMapLevel] = static_cast<unsigned int>(mSegs.size());
   mSegCount[aMapLevel] = 0;
   mPntCount[aMapLevel] = static_cast<unsigned int>(mPnts.size());
   // map later...

   size_t bytesRead = 0;
   while ((bytesRead = fread((char*)rbuf.data(), 1, rbuf.size() * sizeof(WdbFilePnt), filePtr)) > 0)
   {
#if defined(_WIN32)
      _swab((char*)rbuf.data(), (char*)rbuf.data(), static_cast<int>(bytesRead));
#elif defined(__i386) || defined(__x86_64)
      swab((char*)rbuf.data(), (char*)rbuf.data(), bytesRead);
#elif defined(__alpha) && defined(__unix__)
      swab((char*)rbuf.data(), (char*)rbuf.data(), bytesRead);
#endif
      auto pts_in_buf = static_cast<unsigned int>(bytesRead / sizeof(WdbFilePnt));

      for (unsigned int i = 0; i < pts_in_buf; ++i)
      {
         // units in file are in nautical miles -- convert to degrees
         float lat = rbuf[i].lat / 60.0F;
         float lon = rbuf[i].lon / 60.0F;

         if (rbuf[i].code > 5)
         {
            if (segment_active)
            {
               mSegs.push_back(cur_seg);
               segment_active = false;
            }

            int layer = rbuf[i].code / 1000;

            // Ignore the Berlin corridor.  It is classified as an internal
            // border (50N-54N, 10E-14E)

            if (layer == 4)
            {
               if ((lat > 50.0F) && (lat < 54.0F) && (lon > 10.0F) && (lon < 14.0F))
                  continue;
            }

            unsigned short int segType    = MtContinents;
            unsigned short int colorIndex = 0;
            switch (layer)
            {
            case 1:
               segType    = MtContinents;
               colorIndex = 0;
               break;
            case 5:
               segType    = MtIslands;
               colorIndex = 1;
               break;
            case 2:
               segType    = MtCountryBorders;
               colorIndex = 2;
               break;
            case 4:
               segType    = MtInternalBorders;
               colorIndex = 3;
               break;
            case 6:
               segType    = MtLakes;
               colorIndex = 4;
               break;
            case 7:
               segType    = MtRivers;
               colorIndex = 5;
               break;
               //  case 3:         // ??????
            default:
               segType    = MtAll;
               colorIndex = 6;
               break;
            }
            cur_seg.mType        = segType;
            cur_seg.mColor       = colorIndex;
            cur_seg.mIndex       = static_cast<unsigned int>(mPnts.size());
            cur_seg.mCount       = 0;
            cur_seg.mMinLat      = 90.0F;
            cur_seg.mMaxLat      = -90.0F;
            cur_seg.mMinLon      = 180.0F;
            cur_seg.mMaxLon      = -180.0F;
            cur_seg.mDisplayList = 0;
            segment_active       = true;
            last_lat             = lat;
            last_lon             = lon;
         }

         if (!segment_active)
            continue;

         // If the line segment crosses the dateline, break it at the dateline.
         if (fabs(last_lon - lon) > 180.0)
         { // crosses the dateline
            float lon_cross;
            float lon_temp;
            float next_lon;
            if (last_lon < 0.0F)
            { // westbound crossing
               lon_cross = -179.999f;
               next_lon  = 179.999f;
               lon_temp  = lon - 360.0F;
            }
            else
            { // eastbound crossing
               lon_cross = 179.999f;
               next_lon  = -179.999f;
               lon_temp  = lon + 360.0F;
            }
            float t_cross   = (lon_cross - last_lon) / (lon_temp - last_lon);
            float lat_cross = last_lat + (t_cross * (lat - last_lat));

            // put the final point on the outgoing segment
            if (lat_cross < cur_seg.mMinLat)
               cur_seg.mMinLat = lat_cross;
            if (lat_cross > cur_seg.mMaxLat)
               cur_seg.mMaxLat = lat_cross;
            if (lon_cross < cur_seg.mMinLon)
               cur_seg.mMinLon = lon_cross;
            if (lon_cross > cur_seg.mMaxLon)
               cur_seg.mMaxLon = lon_cross;
            cur_seg.mCount += 1;
            mSegs.push_back(cur_seg);
            mPnts.emplace_back(lat_cross, lon_cross);

            // start a new segment with the incoming point.
            cur_seg.mIndex  = static_cast<unsigned int>(mPnts.size());
            cur_seg.mCount  = 1;
            cur_seg.mMinLat = cur_seg.mMaxLat = lat_cross;
            cur_seg.mMinLon = cur_seg.mMaxLon = next_lon;
            mPnts.emplace_back(lat_cross, next_lon);
         }
         else if (((last_lon < -30) && (lon > -30)) || ((last_lon > -30) && (lon < -30)))
         // otherwise look if we want to split at 30w (supports pacific centered maps)
         {
            float lon_cross = -29.999f;
            float next_lon  = -30.001f;
            if (last_lon < -30)
            {
               lon_cross = -30.001f;
               next_lon  = -29.999f;
            }
            float factor    = (last_lon + 30.0f) / (last_lon - lon);
            float lat_cross = factor * lat + (1 - factor) * last_lat;

            // put the final point on the outgoing segment
            if (lat_cross < cur_seg.mMinLat)
               cur_seg.mMinLat = lat_cross;
            if (lat_cross > cur_seg.mMaxLat)
               cur_seg.mMaxLat = lat_cross;
            if (lon_cross < cur_seg.mMinLon)
               cur_seg.mMinLon = lon_cross;
            if (lon_cross > cur_seg.mMaxLon)
               cur_seg.mMaxLon = lon_cross;
            cur_seg.mCount += 1;

            mSegs.push_back(cur_seg);
            mPnts.emplace_back(lat_cross, lon_cross);

            cur_seg.mIndex  = static_cast<unsigned int>(mPnts.size());
            cur_seg.mCount  = 1;
            cur_seg.mMinLat = cur_seg.mMaxLat = lat_cross;
            cur_seg.mMinLon = cur_seg.mMaxLon = next_lon;
            mPnts.emplace_back(lat_cross, next_lon);
         }

         mPnts.emplace_back(lat, lon);
         ++cur_seg.mCount;
         if (lat < cur_seg.mMinLat)
            cur_seg.mMinLat = lat;
         if (lat > cur_seg.mMaxLat)
            cur_seg.mMaxLat = lat;
         if (lon < cur_seg.mMinLon)
            cur_seg.mMinLon = lon;
         if (lon > cur_seg.mMaxLon)
            cur_seg.mMaxLon = lon;
         last_lat = lat;
         last_lon = lon;
      }
   }

   if (segment_active)
   { // finish the last segment
      mSegs.push_back(cur_seg);
   }

   fclose(filePtr);

   mSegCount[aMapLevel] = static_cast<unsigned int>(mSegs.size()) - mSegIndex[aMapLevel];
   mPntCount[aMapLevel] = static_cast<unsigned int>(mPnts.size()) - mPntCount[aMapLevel];
}

void VaOverlayWdb::DrawMap(VaCamera* aCameraPtr)
{
   glDisable(GL_LIGHTING);
   double easternOffsetStart = aCameraPtr->GetEasternOffsetStart();
   double easternOffsetValue = aCameraPtr->GetEasternOffsetValue();
   double westernOffsetStart = aCameraPtr->GetWesternOffsetStart();
   double westernOffsetValue = aCameraPtr->GetWesternOffsetValue();
   double offset1;
   double offset2;

   GeoRect viewRect;
   int     numCycles = aCameraPtr->GetViewableArea(viewRect);

   if (aCameraPtr->IsFlat())
   {
      if (numCycles > 0)
      {
         viewRect.SetSWLon(-180.0);
         viewRect.SetNELon(180.0);
      }
   }

   unsigned int beg_seg = mSegIndex[mMapLevel];
   unsigned int end_seg = beg_seg + mSegCount[mMapLevel];
   for (unsigned int seg = beg_seg; seg < end_seg; ++seg)
   {
      // Don't draw this segment unless it is one of the requested types

      if ((mSegs[seg].mType & mActiveLayers) == 0)
      {
         continue;
      }

      // Cull out the segments that won't be visible.  This speeds up drawing
      // considerably when zoomed in close.

      GeoRect segRect(mSegs[seg].mMinLat, mSegs[seg].mMinLon, mSegs[seg].mMaxLat, mSegs[seg].mMaxLon);
      if (!segRect.Intersects(viewRect))
      {
         continue;
      }
      if (mSegs[seg].mDisplayList == 0)
      {
         MakeDisplayList(aCameraPtr, mSegs[seg]);
      }

      offset1       = 0.0;
      offset2       = 0.0;
      double minLon = mSegs[seg].mMinLon;
      if (minLon > easternOffsetStart)
      {
         offset1 = easternOffsetValue;
      }
      else if (minLon < westernOffsetStart)
      {
         offset1 = westernOffsetValue;
      }
      double maxLon = mSegs[seg].mMaxLon;
      if (maxLon > easternOffsetStart)
      {
         offset2 = easternOffsetValue;
      }
      else if (maxLon < westernOffsetStart)
      {
         offset2 = westernOffsetValue;
      }
      if ((offset1 == 0.0) && (offset2 != 0.0))
      {
         offset1 = offset2;
         offset2 = 0.0;
      }

      if (offset1 == 0.0)
      {
         glCallList(mSegs[seg].mDisplayList);
      }
      else
      {
         glPushMatrix();
         glTranslated(offset1, 0.0, 0.0);
         glCallList(mSegs[seg].mDisplayList);
         glPopMatrix();
         if (offset1 != offset2)
         {
            glCallList(mSegs[seg].mDisplayList);
         }
      }
   }
}

void VaOverlayWdb::MakeDisplayList(VaCamera* aCameraPtr, WdbSeg& aSegment)
{
   double  xyzd[3];
   GLfloat xyzf[3];

   aSegment.mDisplayList = glGenLists(1);
   glNewList(aSegment.mDisplayList, GL_COMPILE);

   glColor3fv(mColorMap[aSegment.mColor]);
   glLineWidth(mWidthMap[aSegment.mColor]);
   glBegin(GL_LINE_STRIP);
   unsigned int beg_pnt = aSegment.mIndex;
   unsigned int end_pnt = beg_pnt + aSegment.mCount;
   for (unsigned int pnt = beg_pnt; pnt < end_pnt; ++pnt)
   {
      aCameraPtr->ConvertLLAToXYZ(xyzd, mPnts[pnt].mLat, mPnts[pnt].mLon, 0.0);
      xyzf[0] = static_cast<float>(xyzd[0]);
      xyzf[1] = static_cast<float>(xyzd[1]);
      xyzf[2] = static_cast<float>(xyzd[2]);
      glVertex3fv(xyzf);
   }
   glEnd();

   glEndList();
}

void VaOverlayWdb::SetColor(const std::string& aLayerId, const float* aColor)
{
   if (aLayerId == "Country_Borders")
   {
      mColorMap[2][0] = aColor[0];
      mColorMap[2][1] = aColor[1];
      mColorMap[2][2] = aColor[2];
   }
   else if (aLayerId == "Coast_Lines")
   {
      mColorMap[0][0] = aColor[0];
      mColorMap[0][1] = aColor[1];
      mColorMap[0][2] = aColor[2];
      mColorMap[1][0] = aColor[0];
      mColorMap[1][1] = aColor[1];
      mColorMap[1][2] = aColor[2];
   }
   else if (aLayerId == "Internal_Borders")
   {
      mColorMap[3][0] = aColor[0];
      mColorMap[3][1] = aColor[1];
      mColorMap[3][2] = aColor[2];
   }
   else if (aLayerId == "Lakes_and_Rivers")
   {
      mColorMap[4][0] = aColor[0];
      mColorMap[4][1] = aColor[1];
      mColorMap[4][2] = aColor[2];
      mColorMap[5][0] = aColor[0];
      mColorMap[5][1] = aColor[1];
      mColorMap[5][2] = aColor[2];
   }
   ResetWdbData();
}

void VaOverlayWdb::SetWidth(const std::string& aLayerId, const float& aWidth)
{
   if (aLayerId == "Country_Borders")
   {
      mWidthMap[2] = aWidth;
   }
   else if (aLayerId == "Coast_Lines")
   {
      mWidthMap[0] = aWidth;
      mWidthMap[1] = aWidth;
   }
   else if (aLayerId == "Internal_Borders")
   {
      mWidthMap[3] = aWidth;
   }
   else if (aLayerId == "Lakes_and_Rivers")
   {
      mWidthMap[4] = aWidth;
      mWidthMap[5] = aWidth;
   }
   ResetWdbData();
}

void VaOverlayWdb::ShowLayer(const std::string& aLayerId)
{
   bool show = (mActiveLayers == 0);

   if (aLayerId == "Coast_Lines")
   {
      mActiveLayers |= MtIslands | MtContinents;
   }
   else if (aLayerId == "Lakes_and_Rivers")
   {
      mActiveLayers |= MtLakes | MtRivers;
   }

   if (show)
   {
      SetVisible(true);
   }
}

void VaOverlayWdb::HideLayer(const std::string& aLayerId)
{
   if (aLayerId == "Coast_Lines")
   {
      mActiveLayers &= ~(MtIslands | MtContinents);
   }
   else if (aLayerId == "Lakes_and_Rivers")
   {
      mActiveLayers &= ~(MtLakes | MtRivers);
   }
   if (mActiveLayers == 0)
   {
      SetVisible(false);
   }
}

void VaOverlayWdb::MapProjectionChangedCB(unsigned int aViewer, const UtoMapProjection* aProjection)
{
   if (aProjection->projection == UtoMapProjection::Geocentric)
   {
      mGeocentric = true;
   }
   else
   {
      mGeocentric = false;
   }
   GenerateWdbData();
}

} // namespace vespa
