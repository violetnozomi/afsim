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

#ifndef UTLLAPOS_HPP
#define UTLLAPOS_HPP

#include "ut_export.h"

#include <cmath>
#include <iosfwd>

#include "UtLatPos.hpp"
#include "UtLength.hpp"
#include "UtLonPos.hpp"

class UT_EXPORT UtLLAPos
{
public:
   UtLLAPos() = default;

   UtLLAPos(double aLat, double aLon, double aAlt)
      : mLat(aLat)
      , mLon(aLon)
      , mAlt(aAlt)
      , mLatFormat(0)
      , mLonFormat(0)
      , mAltFormat(0)
   {
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
   }

   // Using default copy constructor
   // UtLLAPos(const UtLLAPos& aCoord);

   // Using default assignment operator
   // UtLLAPos& operator=(const UtLLAPos& aRhs);

   virtual ~UtLLAPos() = default;

   // Determines if to locations are coincident (within some small distance)
   bool operator==(const UtLLAPos& aRhs) const
   {
      const double angEps = (1.0 / 3600000.0); // .001 sec ~= 3cm at equator
      const double altEps = 0.01;              // 1cm
      return ((fabs(mLat - aRhs.mLat) < angEps) && (fabs(mLon - aRhs.mLon) < angEps) && (fabs(mAlt - aRhs.mAlt) < altEps));
   }

   UtLLAPos operator+(const UtLLAPos& aRhs) { return UtLLAPos(mLat + aRhs.mLat, mLon + aRhs.mLon, mAlt + aRhs.mAlt); }

   UtLLAPos operator-(const UtLLAPos& aRhs) { return UtLLAPos(mLat - aRhs.mLat, mLon - aRhs.mLon, mAlt - aRhs.mAlt); }

   UtLLAPos& operator+=(const UtLLAPos& aRhs)
   {
      mLat += aRhs.mLat;
      mLon += aRhs.mLon;
      mAlt += aRhs.mAlt;
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

   UtLLAPos& operator-=(const UtLLAPos& aRhs)
   {
      mLat -= aRhs.mLat;
      mLon -= aRhs.mLon;
      mAlt -= aRhs.mAlt;
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
   double GetAlt() const { return mAlt; }

   void GetLat(UtLatPos& aLat) const { aLat.Set(mLat, mLatFormat); }
   void GetLon(UtLonPos& aLon) const { aLon.Set(mLon, mLonFormat); }
   void GetAlt(UtLength& aAlt) const { aAlt.Set(mAlt, mAltFormat); }

   void SetLat(double aLat) { mLat = aLat; }
   void SetLon(double aLon)
   {
      mLon = aLon;
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
   }
   void SetAlt(double aAlt) { mAlt = aAlt; }

   int  GetLatFormat() const { return mLatFormat; }
   int  GetLonFormat() const { return mLonFormat; }
   int  GetAltFormat() const { return mAltFormat; }
   void SetLatFormat(int aLatFormat) { mLatFormat = aLatFormat; }
   void SetLonFormat(int aLonFormat) { mLonFormat = aLonFormat; }
   void SetAltFormat(int aAltFormat) { mAltFormat = aAltFormat; }

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
   void SetAlt(const UtLength& aAlt)
   {
      mAlt       = aAlt;
      mAltFormat = aAlt.GetFormat();
   }

   void Set(double aLat, double aLon, double aAlt)
   {
      mLat = aLat;
      mLon = aLon;
      mAlt = aAlt;
      if (mLon > 180.0)
         mLon -= 360.0;
      if (mLon <= -180.0)
         mLon += 360.0;
   }

   // stream extractor
   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtLLAPos& aLLA);

   // stream inserter
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtLLAPos& aLLA);

private:
   double mLat{0.0};
   double mLon{0.0};
   double mAlt{0.0};
   int    mLatFormat{0};
   int    mLonFormat{0};
   int    mAltFormat{0};
};

#endif
