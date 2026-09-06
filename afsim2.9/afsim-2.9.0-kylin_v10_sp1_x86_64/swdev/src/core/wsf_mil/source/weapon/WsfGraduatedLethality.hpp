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

#ifndef WSFGRADUATEDLETHALITY_HPP
#define WSFGRADUATEDLETHALITY_HPP

#include "wsf_mil_export.h"

#include <vector>

#include "WsfExplicitWeaponEffects.hpp"

//! A weapon effect that maps decreasing Probability of Kill (Pk) into concentric spheres radiating
//! away from the blast center.
//!
//! This class is a specialization of WsfExplicitWeaponEffects that
//! implements tables of decreasing target Pk as the radius from weapon blast center increases.
//! The target kill determination will be digital (all or nothing), according to a normal random
//! draw.  The transfer function may be specified as discrete, with Pk dropping in stair-steps
//! further from the weapon, or linearly interpolated between breakpoints.  Different target
//! platform types or sub-types may have their own transfer functions to account for varying
//! target harness, or a 'default' transfer function may be defined which applies to all target
//! types not otherwise included in a separate table.  The base type of every target must be
//! WSF_PLATFORM, so the 'default' lethality equates to target_type WSF_PLATFORM, and Pk is zero,
//! unless populated with non-zero values by stream input.

class WSF_MIL_EXPORT WsfGraduatedLethality : public WsfExplicitWeaponEffects
{
public:
   WsfGraduatedLethality(WsfScenario& aScenario);
   ~WsfGraduatedLethality() override = default;
   WsfWeaponEffects* Clone() const override;
   bool              PreInitialize() override;
   bool              ProcessInput(UtInput& aInput) override;

   //! The Point class is not of concern to the end user.  It should have
   //! been private, but the SUN compiler requires that it be made public.
   class Point
   {
   public:
      Point()
         : mRSqrd(0.0)
         , mPk(0.0)
      {
      }
      Point(double aRadius, double aPk)
         : mRSqrd(aRadius * aRadius)
         , mPk(aPk)
      {
      }
      double RadSqrd() const { return mRSqrd; }
      double Pk() const { return mPk; }
      void   Set(double aRadius, double aPk)
      {
         mRSqrd = aRadius * aRadius;
         mPk    = aPk;
      }
      // operator< is required for sort() algorithm.
      bool operator<(const Point& rhs) const { return mRSqrd < rhs.mRSqrd; }

   private:
      double mRSqrd;
      double mPk;
   };

   using Points          = std::vector<Point>;
   using PointsIter      = std::vector<Point>::iterator;
   using PointsConstIter = std::vector<Point>::const_iterator;

   //! The Table class is not of concern to the end user.  It should have
   //! been private, but the SUN compiler requires that it be made public.
   class Table
   {
   public:
      Table();
      Table(WsfStringId aTypeId);
      Table(const Table& aSrc) = default;
      Table&        operator=(const Table&) = delete;
      WsfStringId   TypeId() const { return mTypeId; }
      unsigned int  Size() const { return static_cast<unsigned int>(mValues.size()); }
      double        GetPk(double aMissDistSquared, bool aInterpolate) const;
      const Point&  Pt(unsigned int aIndex) const { return mValues[aIndex]; }
      const Points& Pts() const { return mValues; }
      void          AddPoint(const Point& aPoint) { mValues.push_back(aPoint); }
      void          SetPoint(unsigned int aIndex, double aRadius, double aPk) { mValues[aIndex].Set(aRadius, aPk); }
      bool          Sort(double& aMaxRSquared);

   private:
      WsfStringId mTypeId;
      bool        mCanInterpolate; // Interpolation is possible (if requested)
      Points      mValues;
   };

   using Tables = std::vector<Table>;

protected:
   WsfGraduatedLethality(const WsfGraduatedLethality& aSrc) = default;

   //! Prevent use of operator= by declaring, but not defining.
   WsfGraduatedLethality& operator=(const WsfGraduatedLethality& aRhs) = delete;

   void CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) override;

private:
   void   AddPoint(double aRadius, double aPk);
   void   FindTable(WsfPlatform* aTargetPtr);
   double GetPk(double aMissDistSquared) const;
   void   ProcessBlock(UtInput& aInput);
   bool   ReadPkAndRadius(UtInput& aInput);
   bool   TableExistsForType(WsfStringId aTypeId) const;

   // Flag indicates that the miss radii are 2-D rather than 3-D (default = false).
   bool m2DOffsets;

   // Flag indicates that the Pk's are continuously interpolated,
   // versus discrete (the default).
   bool mPkInterp;
   bool mDefIsNull;

   Tables       mTables;
   unsigned int mTableIndex;
};

#endif
