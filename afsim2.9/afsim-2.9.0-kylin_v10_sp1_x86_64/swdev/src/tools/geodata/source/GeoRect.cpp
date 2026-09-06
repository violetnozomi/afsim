// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "GeoRect.hpp"

// virtual
GeoRect::~GeoRect() {}

// Does the rectangle contain the specified point?

int GeoRect::Contains(double aLat, double aLon) const
{
   int contains = 0;

   // We compare latitude first before checking longitude because the check
   // for longitude has to consider the possibility the rectangle may span
   // the dateline.  If the point is not within the latitude bounds then we
   // can completely bypass the more expensive longitude test.

   if ((aLat >= mSWLat) && (aLat <= mNELat))
   {
      if (mSWLon < mNELon)
      {
         // This rectangle does not span the dateline
         if ((aLon >= mSWLon) && (aLon <= mNELon))
         {
            contains = 1;
         }
      }
      else
      {
         // This rectangle spans the dateline
         if (aLon >= 0.0)
         {
            if (aLon >= mSWLon)
            {
               contains = 1;
            }
         }
         else
         {
            if (aLon <= mNELon)
            {
               contains = 1;
            }
         }
      }
   }
   return contains;
}

// Does the rectangle wholly contain the specified rectangle

int GeoRect::Contains(const GeoRect& aRect) const
{
   int contains = 0;
   if (Contains(aRect.mSWLat, aRect.mSWLon) && Contains(aRect.mNELat, aRect.mSWLon) &&
       Contains(aRect.mNELat, aRect.mNELon) && Contains(aRect.mSWLat, aRect.mNELon))
   {
      contains = 1;
   }
   return contains;
}

// Does the specified region intersect the rectangle

int GeoRect::Intersects(const GeoRect& aRect) const
{
   int intersects = 0;

   // We compare latitude first before checking longitude because the check
   // for longitude has to consider the possibility either rectangle may span
   // the dateline.  If the rectangles can intersect within the latitude
   // bounds then we can completely bypass the more expensive longitude test.

   if ((aRect.mSWLat <= mNELat) && (aRect.mNELat >= mSWLat))
   {
      if (mSWLon <= mNELon)
      {
         // This rectangle does not span the dateline.
         if (aRect.mSWLon <= aRect.mNELon)
         {
            // Neither rectangle spans the dateline
            if ((aRect.mSWLon <= mNELon) && (aRect.mNELon >= mSWLon))
            {
               intersects = 1;
            }
         }
         else
         {
            // This rectangle doesn't span the dateline but the other does.
            if ((aRect.mNELon >= mSWLon) || (aRect.mSWLon <= mNELon))
            {
               intersects = 1;
            }
         }
      }
      else
      {
         // This rectangle spans the dateline
         if (aRect.mSWLon <= aRect.mNELon)
         {
            // This rectangle spans the date line but the other does not.
            if ((mNELon >= aRect.mSWLon) || (mSWLon <= aRect.mNELon))
            {
               intersects = 1;
            }
         }
         else
         {
            // Both rectangles span the dateline.
            intersects = 1;
         }
      }
   }
   return intersects;
}

// 'Normalize' out of range values so they are in the proper range.
// Latitudes are clamped so they are between +/-90.  Longitudes will
// have +/- 360 added until they are in the range +/-90.

void GeoRect::Normalize()
{
   // Clamp latitudes to [-90, 90].

   if (mSWLat < -90.0)
   {
      mSWLat = -90.0;
   }
   else if (mSWLat > 90.0)
   {
      mSWLat = 90.0;
   }

   if (mNELat < -90.0)
   {
      mNELat = -90.0;
   }
   else if (mNELat > 90.0)
   {
      mNELat = 90.0;
   }

   // Normalize longitudes to the range [-180, 180].

   while (mSWLon < -180.0)
   {
      mSWLon += 360.0;
   }
   while (mSWLon > 180.0)
   {
      mSWLon -= 360.0;
   }

   while (mNELon < -180.0)
   {
      mNELon += 360.0;
   }
   while (mNELon > 180.0)
   {
      mNELon -= 360.0;
   }
}
