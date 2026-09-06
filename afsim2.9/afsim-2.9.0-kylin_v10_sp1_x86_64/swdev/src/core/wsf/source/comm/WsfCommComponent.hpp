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

#ifndef WSFCOMMCOMPONENT_HPP
#define WSFCOMMCOMPONENT_HPP

#include "wsf_export.h"

#include "WsfComponent.hpp"

class WsfMessage;
class WsfPlatform;
class WsfSimulation;

namespace wsf
{
namespace comm
{

class Address;
class Comm;
class Message;
class Result;

//! A abstract base class for comm system extension components.
//! Components can be added to implement additional functionality beyond what is provided
//! by a specific model. The responsibilities of a derived class include:
//! -) Implementing the 'required interface' methods as defined in WsfComponent.
//! -) Implementing any desired 'common framework' methods defined in WsfComponent.
//! -) Implementing any desired methods as defined below.
//! -) Implementing the specific capabilities required by the derived class.
class WSF_EXPORT Component : public WsfComponentT<Comm>
{
public:
   Component()           = default;
   ~Component() override = default;

   //! @name Utility methods.
   //@{
   Comm*          GetComm() const { return GetComponentParent(); }
   WsfPlatform*   GetPlatform() const;
   WsfSimulation* GetSimulation() const;
   //@}

   //! @name Generic methods.
   //!
   //! Actual comm model implementations do not need to worry about calling these directly
   //! because they are required to call base class methods that end up calling these methods.
   //! (e.g. WsfCommXmtrRcvr::ProcessInput calls WsfComm::ProcessInput).
   //!
   //! These always have the same argument lists as the routine from which they are called.
   //@{
   using WsfComponent::Initialize;
   using WsfComponent::Initialize2;
   using WsfComponent::PreInitialize;
   using WsfComponent::ProcessInput;
   virtual void TurnOn(double aSimTime) {}
   virtual void TurnOff(double aSimTime) {}
   virtual void Setup() {}
   virtual void PendingStart() {}
   //@}

   //! @name Model-specific methods.
   //!
   //! Actual comm models (e.g. WsfCommXmtrRcvr, WsfRadioXmtrRcvr, etc.) must place calls in
   //! their code because there is no common location from where these methods could be called.
   //! See the static wrapper methods below.
   //@{

   virtual void AttemptToTransmit(double aSimTime, Result& aResult) {}

   virtual void AttemptToReceive(double aSimTime, Result& aResult) {}

   virtual bool CanInteractWith(const Comm* aXmtrPtr);

   virtual bool Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage) { return true; }
   virtual bool Send(double aSimTime, const WsfMessage& aMessage, const Address& aAddress) { return true; }

   //@}

   //! @name Model-specific wrapper methods.
   //! These wrap the model-specific methods so specific models only need to have one line
   //! inserted into their code where component effects need to be evaluated.
   //@{
   static void AttemptToTransmit(Comm& aComm, double aSimTime, Result& aResult);

   static void AttemptToReceive(Comm& aComm, double aSimTime, Result& aResult);

   static bool Receive(Comm& aComm, double aSimTime, Comm* aXmtrPtr, Message& aMessage);
   static bool Send(Comm& aComm, double aSimTime, const WsfMessage& aMessage, const Address& aAddress);
   //@}
};

} // namespace comm
} // namespace wsf

#endif
