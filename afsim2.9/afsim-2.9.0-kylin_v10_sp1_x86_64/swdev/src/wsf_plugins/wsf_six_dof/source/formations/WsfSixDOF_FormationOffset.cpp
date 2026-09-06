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

#include "WsfSixDOF_FormationOffset.hpp"

#include <cmath>

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"

wsf::six_dof::FormationOffset wsf::six_dof::FormationOffset::operator+(const FormationOffset& aOther) const
{
   FormationOffset retval{*this};
   retval += aOther;
   return retval;
}

wsf::six_dof::FormationOffset& wsf::six_dof::FormationOffset::operator+=(const FormationOffset& aOther)
{
   mDisplacement += aOther.mDisplacement;
   return *this;
}

wsf::six_dof::FormationOffset& wsf::six_dof::FormationOffset::operator-=(const FormationOffset& aOther)
{
   mDisplacement -= aOther.mDisplacement;
   return *this;
}

bool wsf::six_dof::FormationOffset::ProcessInput(UtInput& aInput)
{
   double inputRange;
   aInput.ReadValueOfType(inputRange, UtInput::cLENGTH);
   double inputRelativeBearing;
   aInput.ReadValueOfType(inputRelativeBearing, UtInput::cANGLE);
   double inputStack;
   aInput.ReadValueOfType(inputStack, UtInput::cLENGTH);
   SetRangeAndBearing(inputRange, inputRelativeBearing);
   SetStack(inputStack);

   std::string command;
   aInput.ReadCommand(command);
   if (command == "fixed_altitude")
   {
      SetWelded(false);
   }
   else if (command != "welded_wing")
   {
      throw UtInput::BadValue(aInput, "Supported offset modes are 'welded_wing' and 'fixed_altitude'");
   }

   return true;
}

double wsf::six_dof::FormationOffset::GetRelativeBearing() const
{
   return atan2(mDisplacement[1], mDisplacement[0]);
}

double wsf::six_dof::FormationOffset::GetDistance() const
{
   // NOTE: This ignores the curvature in LLA coordinates.
   return mDisplacement.Magnitude();
}

double wsf::six_dof::FormationOffset::GetRange() const
{
   // NOTE: This ignores the curvature in LLA coordinates.
   return sqrt(mDisplacement[0] * mDisplacement[0] + mDisplacement[1] * mDisplacement[1]);
}

//! Return if the offset is zero.
bool wsf::six_dof::FormationOffset::IsZero() const
{
   return mDisplacement == 0.0;
}

//! Swap a rightward offset for a leftward, and vice versa.
//! This will switch right and left offsets. This does not modify the values of the stack
//! or how far ahead this offset is.
void wsf::six_dof::FormationOffset::Reflect()
{
   mDisplacement[1] *= -1.0;
}

//! Set the value of the stack.
//!
//! \param aStackInMeters - The stack value in meters, with positive stack meaning higher altitude.
void wsf::six_dof::FormationOffset::SetStack(double aStackInMeters)
{
   mDisplacement[2] = -aStackInMeters;
}

//! Set the displacement rightward.
//!
//! \param aRightInMeters - The size of the rightward displacement in meters.
void wsf::six_dof::FormationOffset::SetRight(double aRightInMeters)
{
   mDisplacement[1] = aRightInMeters;
}

//! Set the displacement ahead.
//!
//! \param aAheadInMeters - The size of the forward displacement in meters.
void wsf::six_dof::FormationOffset::SetAhead(double aAheadInMeters)
{
   mDisplacement[0] = aAheadInMeters;
}

//! Set the range and relative bearing of the offset.
//!
//! \param aRangeInMeters            - the distance to offset.
//! \param aRelativeBearingInRadians - the relative bearing of the offset.
void wsf::six_dof::FormationOffset::SetRangeAndBearing(double aRangeInMeters, double aRelativeBearingInRadians)
{
   mDisplacement[0] = aRangeInMeters * cos(aRelativeBearingInRadians);
   mDisplacement[1] = aRangeInMeters * sin(aRelativeBearingInRadians);
}

//! Set the welded-wing flag.
void wsf::six_dof::FormationOffset::SetWelded(bool aWelded)
{
   mWelded = aWelded;
}
