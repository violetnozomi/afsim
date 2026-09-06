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

#ifndef UTLLPOS_HPP
#define UTLLPOS_HPP

#include "ut_export.h"

#include <cmath>
#include <iosfwd>

#include "UtLatPos.hpp"
#include "UtLonPos.hpp"

class UT_EXPORT UtLLPos
{
public:
   UtLLPos()
      : mLat(0.0)
      , mLon(0.0)
      , mLatFormat(0)
      , mLonFormat(0)
   {
   }

   UtLLPos(double aLat, double aLon)
      : mLat(aLat)
      , mLon(aLon)
      , mLatFormat(0)
      , mLonFormat(0)
   {
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
   }

   // Using default copy constructor
   // UtLLPos(const UtLLPos& aCoord);

   // Using default assignment operator
   // UtLLPos& operator=(const UtLLPos& aRhs);

   virtual ~UtLLPos() = default;

   // Determines if to locations are coincident (within some small distance)
   bool operator==(const UtLLPos& aRhs) const
   {
      const double angEps = (1.0 / 3600000.0); // .001 sec ~= 3cm at equator
      return ((fabs(mLat - aRhs.mLat) < angEps) && (fabs(mLon - aRhs.mLon) < angEps));
   }

   bool operator!=(const UtLLPos& aRhs) const { return !(operator==(aRhs)); }

   UtLLPos operator+(const UtLLPos& aRhs) { return UtLLPos(mLat + aRhs.mLat, mLon + aRhs.mLon); }

   UtLLPos operator-(const UtLLPos& aRhs) { return UtLLPos(mLat - aRhs.mLat, mLon - aRhs.mLon); }

   UtLLPos& operator+=(const UtLLPos& aRhs)
   {
      mLat += aRhs.mLat;
      mLon += aRhs.mLon;
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
      if (mLat >= 89.9999)
      {
         mLat = 180.0 - mLat;
         if (mLat > 89.9999)
            mLat = 89.9999;
         mLon = 180.0 - mLon;
         if (mLon > 180.0)
            mLon -= 180.0;
         if (mLon <= -180.0)
            mLon += 180.0;
      }
      else if (mLat <= -89.9999)
      {
         mLat = -180.0 - mLat;
         if (mLat < -89.9999)
            mLat = -89.9999;
         mLon = 180.0 - mLon;
         if (mLon > 180.0)
            mLon -= 180.0;
         if (mLon <= -180.0)
            mLon += 180.0;
      }
      return *this;
   }

   UtLLPos& operator-=(const UtLLPos& aRhs)
   {
      mLat -= aRhs.mLat;
      mLon -= aRhs.mLon;
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
      if (mLat >= 89.9999)
      {
         mLat = 180.0 - mLat;
         if (mLat > 89.9999)
            mLat = 89.9999;
         mLon = 180.0 - mLon;
         if (mLon > 180.0)
            mLon -= 180.0;
         if (mLon <= -180.0)
            mLon += 180.0;
      }
      else if (mLat <= -89.9999)
      {
         mLat = -180.0 - mLat;
         if (mLat < -89.9999)
            mLat = -89.9999;
         mLon = 180.0 - mLon;
         if (mLon > 180.0)
            mLon -= 180.0;
         if (mLon <= -180.0)
            mLon += 180.0;
      }
      return *this;
   }

   double GetLat() const { return mLat; }
   double GetLon() const { return mLon; }

   void SetLat(double aLat) { mLat = aLat; }
   void SetLon(double aLon)
   {
      mLon = aLon;
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
   }

   int  GetLatFormat() const { return mLatFormat; }
   int  GetLonFormat() const { return mLonFormat; }
   void SetLatFormat(int aLatFormat) { mLatFormat = aLatFormat; }
   void SetLonFormat(int aLonFormat) { mLonFormat = aLonFormat; }

   void SetLat(const UtLatPos& aLat)
   {
      mLat       = aLat;
      mLatFormat = aLat.GetFormat();
   }
   void SetLon(const UtLonPos& aLon)
   {
      mLon       = aLon;
      mLonFormat = aLon.GetFormat();
   }

   void Set(double aLat, double aLon)
   {
      mLat = aLat;
      mLon = aLon;
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
   }

   // stream operators
   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtLLPos& aLL);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtLLPos& aLL);

private:
   double mLat;
   double mLon;
   int    mLatFormat;
   int    mLonFormat;
};

#endif
