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

#ifndef UTRIC_FRAME_HPP
#define UTRIC_FRAME_HPP

#include "ut_export.h"

#include "UtVec3.hpp"

namespace ut
{

//! A utility class that can be used to convert to and from the RIC frame from an inertial frame.
//! This class provides a set of static methods to allow use in cases where repeated transformations
//! are not needed.
//! The RIC frame is a non-inertial frame that is comoving with an entity that is instantaneously
//! specified with the inertial position and velocity of that entity. The frame uses an x-axis along
//! the position to the entity (Radial), a z-axis along the orbital angular momentum of the entity
//! (Cross-track), and a y-axis that completes the right handed orthonormal frame (In-track). When
//! in a circular orbit, the in-track direction is the same as the direction of the entity's velocity.
class UT_EXPORT RIC_Frame
{
public:
   RIC_Frame(const UtVec3d& aPosition, const UtVec3d& aVelocity);

   //! Update the frame with the given inertial position and velocity.
   void Update(const UtVec3d& aPostion, const UtVec3d& aVelocity);

   //! Compute the inertial position corresponding to the given RIC position.
   UtVec3d PositionFromRIC(const UtVec3d& aRIC_Position) const;

   //! Compute the RIC position corresponding to the given inertial position.
   UtVec3d PositionToRIC(const UtVec3d& aPosition) const;

   //! Compute the inertial velocity from the corresponding RIC kinematics.
   UtVec3d VelocityFromRIC(const UtVec3d& aRIC_Position, const UtVec3d& aRIC_Velocity) const;

   //! Transform the given RIC vector into its inertial equivalent.
   //! Note that this method only rotates the vector from the RIC frame into the
   //! inertial frame, and does not take into account any change or origin, or
   //! non-inertial effects.
   UtVec3d TransformFromRIC(const UtVec3d& aRIC_Vector) const;

   //! Transform the given inertial vector into its RIC equivalent
   //! Note that this method only rotates the vector from the inertial frame into
   //! the RIC frame, and does not take into account any change of origin, or
   //! non-inertial effects.
   UtVec3d TransformToRIC(const UtVec3d& aVector) const;


   // Static utility methods

   //! Calculates the RIC values for a position and velocity with respect to another position.
   //! @param aPosition is the position of the entity relative to which RIC values are being calculated.
   //! @param aVelocity is the velocity of the entity relative to which RIC values are being calculated.
   //! @param aOtherPos is the position of the entity whose RIC values are being calculated.
   //! @returns The RIC frame offsets.
   //! @note  This function is normally used for a space mover wrt another platform,
   //!        but can be used in other domains as well.
   static UtVec3d CalculateToRIC(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtVec3d& aOtherPos);

   //! Calculate the position of an object with the given RIC position.
   //! This will return the position of the of an object that has the given @p aRIC_Position relative
   //! to an object with the given @p aPosition and @p aVelocity. The reference frame of the returned
   //! value will be the same as whatever frame the @p aPosition and @p aVelocity are provided in,
   //! which is typically the ECI frame.
   //! @param aPosition The position of the entity relative to which the RIC value is given.
   //! @param aVelocity The velocity of the entity relative to which the RIC value is given.
   //! @param aOtherPos The RIC frame offset relative to the given @p aPosition and @p aVelocity.
   //! @returns The position corresponding to the given RIC offset.
   static UtVec3d CalculateFromRIC(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtVec3d& aRIC_Position);

   //! Transform the given vector from the RIC frame.
   //! This will return the given @p aRIC_Vector from the RIC frame into the frame used to specify
   //! the @p aPosition and @p aVelcocity used to define the RIC frame. The most common use case would
   //! be for the provided @p aPosition and @p aVelocity to be with respect to the ECI frame, and so
   //! this function would transform the given @p aRIC_Vector into the equivalent ECI vector.
   //! Unlike CalculateFromRIC, this function does not assume that @p aRIC_Vector is a position vector,
   //! and so the returned vector does not offset the returned vector; this functions acts as the
   //! identity operation on the zero vector.
   //! @param aPosition    The position of the entity relative to which the RIC frame vector is given.
   //! @param aVelocity    The velocity of the entity relative to which the RIC frame vector is given.
   //! @param aRIC_Vector  The vector in the RIC frame to be transformed.
   //! @returns The vector corresponding to the given RIC vector in the frame of the provided position and velocity.
   static UtVec3d TransformFromRIC(const UtVec3d& aPosition, const UtVec3d& aVelocity, const UtVec3d& aRIC_Vector);

   //! Transform the given RIC frame velocity vector into an inertial frame.
   //! This will return the inertial equivalent of the given @p aRIC_Velocity. The RIC frame will be computed
   //! from the given @p aPosition and @p aVelocity, both of which are defined with respect to some inertial frame.
   //! The most common use case would be for that inertial frame to be ECI.
   //! @param aPosition     The position of the entity relative to which the RIC frame vector is given.
   //! @param aVelocity     The velocity of the entity relative to which the RIC frame vector is given.
   //! @param aRIC_Position The RIC position at which the relative velocity is needed.
   //! @param aRIC_Velocity The RIC velocity to be transformed into the inertial relative velocity.
   //! @returns The velocity vector in the inertial frame.
   static UtVec3d VelocityFromRIC(const UtVec3d& aPosition,
                                  const UtVec3d& aVelocity,
                                  const UtVec3d& aRIC_Position,
                                  const UtVec3d& aRIC_Velocity);

private:
   UtVec3d mPosition;
   UtVec3d mVelocity;
   UtVec3d mOmega;
   UtVec3d mR_Hat;
   UtVec3d mI_Hat;
   UtVec3d mC_Hat;
};

} // namespace ut

#endif
