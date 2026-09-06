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

#ifndef WSFFORMATIONOFFSET_HPP
#define WSFFORMATIONOFFSET_HPP

#include "wsf_p6dof_export.h"

#include "UtVec3.hpp"

class UtEntity;
class UtInput;

#include "WsfGeoPoint.hpp"

//! An offset from a formation leader to a member of that formation.
//! This value is a 3D vector that contains both an 'in-plane' offset and a
//! 'stack'. The stack is altitude offset relative to the lead, with positive
//! stack being a higher altitude. The 'in-plane' offset is a displacement
//! at the same altitude.
class WSF_P6DOF_EXPORT WsfFormationOffset
{
public:
   WsfFormationOffset()
      : mDisplacement{}
      , mWelded{true}
   {
   }
   WsfFormationOffset(const WsfFormationOffset& aOther) = default;

   WsfFormationOffset& operator=(const WsfFormationOffset& aOther) = default;

   WsfFormationOffset  operator+(const WsfFormationOffset& aOther) const;
   WsfFormationOffset& operator+=(const WsfFormationOffset& aOther);
   WsfFormationOffset& operator-=(const WsfFormationOffset& aOther);

   bool ProcessInput(UtInput& aInput);

   //! Return the stack in meters where positive stack is upwards.
   double GetStack() const { return -mDisplacement[2]; }

   //! Return the distance in meters to right.
   double GetRight() const { return mDisplacement[1]; }

   //! Return the distance in meters ahead.
   double GetAhead() const { return mDisplacement[0]; }

   //! Returns the relative bearing of this offset in radians.
   double GetRelativeBearing() const;

   //! Returns if the offset is to be considered to be wing-welded.
   //! Note that this is only a hint, and it will be ignored for large offsets.
   bool IsWelded() const { return mWelded; }

   double GetDistance() const;
   double GetRange() const;

   bool IsZero() const;

   void Reflect();
   void SetStack(double aStackInMeters);
   void SetRight(double aRightInMeters);
   void SetAhead(double aAheadInMeters);
   void SetRangeAndBearing(double aRangeInMeters, double aRelativeBearingInRadians);
   void SetWelded(bool aWelded);

private:
   UtVec3d mDisplacement; //! The displacement from Lead in Lead's intended body frame.
   bool    mWelded;       //! Is this displacement to be welded-wing.
};

#endif // WSFFORMATIONOFFSET_HPP
