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

#ifndef WSFORBITALDYNAMICSTERM_HPP
#define WSFORBITALDYNAMICSTERM_HPP

#include <string>

#include "UtCalendar.hpp"
#include "UtVec3.hpp"
#include "WsfObject.hpp"
class WsfOrbitalDynamics;

//! A term used in orbital dynamics.
//!
//! A single term in the orbital dynamics is any contribution to the dynamics of a
//! space domain object that is being modeled. A term can be defined in any way that
//! clients find useful, and the only restriction is that they are able to be summed
//! with any other terms that affect the dynamics of the bodies in question.
//!
//! These terms are not intended to hold any state related to the bodies on which these
//! dynamics are acting.
class WsfOrbitalDynamicsTerm : public WsfObject
{
public:
   WsfOrbitalDynamicsTerm()
      : WsfObject{}
   {
   }

   WsfOrbitalDynamicsTerm(const WsfOrbitalDynamicsTerm& aOther)
      : WsfObject(aOther)
      , mDynamicsPtr{nullptr}
   {
   }

   WsfOrbitalDynamicsTerm& operator=(const WsfOrbitalDynamicsTerm& aOther) = delete;

   WsfOrbitalDynamicsTerm* Clone() const override = 0;

   const char* GetScriptClassName() const override { return "WsfOrbitalDynamicsTerm"; }

   //! Return the acceleration in the ECI frame.
   //!
   //! This routine computes the acceleration represented by this term
   //! that would be experienced by a body of the given @p aMass at the
   //! given @p aTime, @p aPosition and @p aVelocity. The given @p aPosition
   //! and @p aVelocity are in the ECI frame, and the returned acceleration
   //! is also in the ECI frame.
   //!
   //! \param aMass     - The mass of the object that would experience this dynamical term.
   //! \param aTime     - The time at which the acceleration is being computed.
   //! \param aPosition - The position of the body that would experience this acceleration.
   //! \param aVelocity - The velocity of the body that would experience this acceleration.
   //! \returns         - The acceleration corresponding to this dynamical term.
   virtual UtVec3d ComputeAcceleration(double            aMass,
                                       const UtCalendar& aTime,
                                       const UtVec3d&    aPosition,
                                       const UtVec3d&    aVelocity) const = 0;

   //! Return a string indicating the type of term.
   virtual std::string GetTermType() const = 0;

   //! Initialize the term.
   //!
   //! This is called when the owning WsfOrbitalDynamics is initialized. This enables
   //! terms that might need to inspect their owning dynamics.
   //!
   //! \param aDynamics - the dynamics to which this term belongs.
   virtual bool Initialize(const WsfOrbitalDynamics& aDynamics)
   {
      mDynamicsPtr = &aDynamics;
      return true;
   }

protected:
   const WsfOrbitalDynamics* GetDynamics() const { return mDynamicsPtr; }

private:
   const WsfOrbitalDynamics* mDynamicsPtr{nullptr};
};

#endif // WSFORBITALDYNAMICSTERM_HPP
