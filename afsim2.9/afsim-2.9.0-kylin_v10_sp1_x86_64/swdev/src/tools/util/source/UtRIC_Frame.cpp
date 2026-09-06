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

#include "UtRIC_Frame.hpp"

namespace ut
{

RIC_Frame::RIC_Frame(const UtVec3d& aPosition, const UtVec3d& aVelocity)
{
   Update(aPosition, aVelocity);
}

void RIC_Frame::Update(const UtVec3d& aPosition, const UtVec3d& aVelocity)
{
   mPosition = aPosition;
   mVelocity = aVelocity;
   mOmega.CrossProduct(mPosition, mVelocity);
   mOmega.Multiply(1.0 / mPosition.MagnitudeSquared());

   mC_Hat.CrossProduct(aPosition, aVelocity);
   mC_Hat.Normalize();
   mI_Hat.CrossProduct(mC_Hat, aPosition);
   mI_Hat.Normalize();
   mR_Hat = aPosition; // up vector
   mR_Hat.Normalize(); // normalized up
}

UtVec3d RIC_Frame::PositionFromRIC(const UtVec3d& aRIC_Position) const
{
   return mPosition + TransformFromRIC(aRIC_Position);
}

UtVec3d RIC_Frame::PositionToRIC(const UtVec3d& aPosition) const
{
   return TransformToRIC(aPosition - mPosition);
}

UtVec3d RIC_Frame::VelocityFromRIC(const UtVec3d& aRIC_Position, const UtVec3d& aRIC_Velocity) const
{
   UtVec3d deltaX = TransformFromRIC(aRIC_Position);
   UtVec3d deltaV = TransformFromRIC(aRIC_Velocity);
   UtVec3d rotTerm;
   rotTerm.CrossProduct(mOmega, deltaX);
   return deltaV + rotTerm + mVelocity;
}

UtVec3d RIC_Frame::TransformFromRIC(const UtVec3d& aRIC_Vector) const
{
   return mR_Hat * aRIC_Vector[0] + mI_Hat * aRIC_Vector[1] + mC_Hat * aRIC_Vector[2];
}

UtVec3d RIC_Frame::TransformToRIC(const UtVec3d& aVector) const
{
   return UtVec3d{mR_Hat.DotProduct(aVector), mI_Hat.DotProduct(aVector), mC_Hat.DotProduct(aVector)};
}

UtVec3d RIC_Frame::CalculateToRIC(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtVec3d& aOtherPos)
{
   RIC_Frame frame{aPosition, aVelocity};
   return frame.PositionToRIC(aOtherPos);
}

UtVec3d RIC_Frame::CalculateFromRIC(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtVec3d& aRIC_Position)
{
   RIC_Frame frame{aPosition, aVelocity};
   return frame.PositionFromRIC(aRIC_Position);
}

UtVec3d RIC_Frame::TransformFromRIC(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtVec3d& aRIC_Vector)
{
   RIC_Frame frame{aPosition, aVelocity};
   return frame.TransformFromRIC(aRIC_Vector);
}

UtVec3d RIC_Frame::VelocityFromRIC(const UtVec3d& aPosition,
                                   const UtVec3d& aVelocity,
                                   const UtVec3d& aRIC_Position,
                                   const UtVec3d& aRIC_Velocity)
{
   RIC_Frame frame{aPosition, aVelocity};
   return frame.VelocityFromRIC(aRIC_Position, aRIC_Velocity);
}

} // namespace ut
