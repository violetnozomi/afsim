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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef GeoRect_hpp
#define GeoRect_hpp

#include "geodata_export.h"

// A 'GeoRect' object defines a rectangular geographic area given by the
// southwest and northeast corners.
//
// The locations are given in decimal degrees as follows:
//
//  -180 (180W) <= Longitude <= +180 (180E)
//   -90 (90S)  <= Latitude  <=  +90 (90N)
//
// NOTE: The rectangle can span the dateline.  In this case the SW longitude
//       is greater than the NE longitude!  BEWARE!
//
// NOTE: Input values are not checked for validity.  There are some cases
//       where callers may manipulate longitudes so as to be monotonical when
//       crossing the dateline.  The containment and intersection checking
//       methods assume the values are in their proper ranges.  If you are
//       not sure the values are in the proper range, call the 'Normalize'
//       method.

class GEODATA_EXPORT GeoRect
{
public:
   GeoRect()
      : mSWLat(0)
      , mSWLon(0)
      , mNELat(0)
      , mNELon(0)
   {
   }

   GeoRect(double aSWLat, double aSWLon, double aNELat, double aNELon)
      : mSWLat(aSWLat)
      , mSWLon(aSWLon)
      , mNELat(aNELat)
      , mNELon(aNELon)
   {
   }

   virtual ~GeoRect();

   // Accessors

   double GetSWLat() const { return mSWLat; }
   double GetSWLon() const { return mSWLon; }
   double GetNELat() const { return mNELat; }
   double GetNELon() const { return mNELon; }

   // Mutators

   void SetSWLat(double aSWLat) { mSWLat = aSWLat; }
   void SetSWLon(double aSWLon) { mSWLon = aSWLon; }
   void SetNELat(double aNELat) { mNELat = aNELat; }
   void SetNELon(double aNELon) { mNELon = aNELon; }

   // Does the rectangle contain the specified point?
   // See the NOTE at the top of this file regarding normalization.

   int Contains(double aLat, double aLon) const;

   // Does the rectangle wholly contain the specified rectangle?
   // See the NOTE at the top of this file regarding normalization.

   int Contains(const GeoRect& aRect) const;

   // Does the specified region intersect the rectangle?
   // See the NOTE at the top of this file regarding normalization.

   int Intersects(const GeoRect& aRect) const;

   // 'Normalize' out of range values so they are in the proper range.
   // Latitudes are clamped so they are between +/-90.  Longitudes will
   // have +/- 360 added until they are in the range +/-180.

   void Normalize();

private:
   double mSWLat;
   double mSWLon;
   double mNELat;
   double mNELon;
};

#endif
