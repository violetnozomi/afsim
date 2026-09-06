// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_SIXDOF_FORMATIONOFFSET_HPP
#define WSF_SIXDOF_FORMATIONOFFSET_HPP

#include "wsf_six_dof_export.h"

#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"

class UtEntity;
class UtInput;

namespace wsf
{
namespace six_dof
{
//! An offset from a formation leader to a member of that formation.
//! This value is a 3D vector that contains both an 'in-plane' offset and a
//! 'stack'. The stack is altitude offset relative to the lead, with positive
//! stack being a higher altitude. The 'in-plane' offset is a displacement
//! at the same altitude.
class WSF_SIX_DOF_EXPORT FormationOffset
{
public:
   FormationOffset()
      : mDisplacement{}
      , mWelded{true}
   {
   }
   FormationOffset(const FormationOffset& aOther) = default;

   FormationOffset& operator=(const FormationOffset& aOther) = default;

   FormationOffset  operator+(const FormationOffset& aOther) const;
   FormationOffset& operator+=(const FormationOffset& aOther);
   FormationOffset& operator-=(const FormationOffset& aOther);

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
   UtVec3d mDisplacement; //! The displacement from lead in lead's intended body frame.
   bool    mWelded;       //! Is this displacement to be welded-wing.
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONOFFSET_HPP
