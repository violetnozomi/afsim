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

#ifndef WSFWEAPONCOMPONENT_HPP
#define WSFWEAPONCOMPONENT_HPP

#include "wsf_mil_export.h"

#include "WsfComponent.hpp"
class WsfPlatform;
#include "WsfSimpleComponent.hpp"
class WsfSimulation;
class WsfWeapon;

//! A abstract base class for processor extension components.
//! Components can be added to implement additional functionality beyond what is provided
//! by a specific model. The responsibilities of a derived class include:
//! -) Implementing the 'required interface' methods as defined in WsfComponent.
//! -) Implementing any desired 'common framework' methods defined in WsfComponent.
//! -) Implementing any desired methods as defined below.
//! -) Implementing the specific capabilities required by the derived class.
class WSF_MIL_EXPORT WsfWeaponComponent : public WsfComponentT<WsfWeapon>
{
public:
   //! @name Utility methods.
   //@{
   WsfWeapon*     GetWeapon() const { return GetComponentParent(); }
   WsfPlatform*   GetPlatform() const;
   WsfSimulation* GetSimulation() const;
   //@}

   //! @name Generic methods.
   //!
   //! Actual processor model implementations do not need to worry about calling these directly
   //! because they are required to call base class methods that end up calling these methods.
   //! (e.g. WsfExplicitWeapon::ProcessInput calls WsfWeapon::ProcessInput).
   //!
   //! These always have the same argument lists as the routine from which they are called.
   //@{
   using WsfComponent::Initialize;
   using WsfComponent::Initialize2;
   using WsfComponent::PreInitialize;
   using WsfComponent::ProcessInput;
   virtual void TurnOn(double aSimTime) {}
   virtual void TurnOff(double aSimTime) {}
   //@}

   //! @name Model-specific methods.
   //!
   //! Actual weapon models (e.g. WsfExplictWeapon, WsfImplicitWeapon, etc.) must place calls in
   //! their code because there is no common location from where these methods could be called.
   //! See the static wrapper methods below.
   //@{
   //@}

   //! @name Model-specific wrapper methods.
   //! These wrap the model-specific methods so specific models only need to have one line
   //! inserted into their code where component effects need to be evaluated.
   //@{
   //@}
};

//! Explicitly instantiate and export template for simple weapon component, and provide convenient alias
template class WSF_MIL_EXPORT WsfSimpleComponentT<WsfWeaponComponent>;
using WsfSimpleWeaponComponent = WsfSimpleComponentT<WsfWeaponComponent>;

#endif
