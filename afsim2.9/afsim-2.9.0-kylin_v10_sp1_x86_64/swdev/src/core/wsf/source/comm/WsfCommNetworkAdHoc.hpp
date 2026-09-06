// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMNETWORKADHOC_HPP
#define WSFCOMMNETWORKADHOC_HPP

#include "wsf_export.h"

#include "WsfCommNetworkGeneric.hpp"
#include "WsfRandomVariable.hpp"
class WsfSimulation;

namespace wsf
{
namespace comm
{

//! NetworkAdHoc is a network type that, like the generic network, has no
//! specific topology associated with it. However, this network type can
//! alter the truth based (network manager) links within the network
//! during the simulation run based on the query of comm model implementation
//! dependent checks on connectivity with peers (via Comm::CanSendTo()).
//! Thus, this network type dynamically updates the simulation truth state
//! during runtime.
//!
//! Note that this is not a replacement for a formal ad-hoc routing protocol.
//! Such protocols are generally reactive based on the discovery (and loss of connectivity)
//! of other nodes, and as such this network type can drive such a protocol.
class WSF_EXPORT NetworkAdHoc : public NetworkGeneric
{
public:
   //! These definitions provide the containers used to associate update
   //! rates for specific members in the network. They can be provided
   //! either via a named comm pair, or an address. An update rate for
   //! a specific member overrides the generic update rate.
   using NamedRateMap   = std::unordered_map<NamedComm, WsfRandomVariable>;
   using AddressRateMap = std::unordered_map<Address, WsfRandomVariable>;

   NetworkAdHoc()           = default;
   ~NetworkAdHoc() override = default;
   NetworkAdHoc(const NetworkAdHoc& aSrc);
   NetworkAdHoc& operator=(const NetworkAdHoc& aRhs);
   NetworkAdHoc(const std::string& aNetworkName, const Address& aAddress);

   bool          ProcessInput(UtInput& aInput) override;
   NetworkAdHoc* Clone() const override;
   const char*   GetScriptClassName() const override { return "WsfNetworkAdHoc"; }
   bool          Initialize(WsfSimulation& aSim) override;

   //! Change the update rate
   //! Note: This only occurs after the next update.
   //! If the update rate is set to never fire, this will schedule
   //! the first update event from the time of this call.
   void ChangeRate(WsfSimulation& aSim, const WsfRandomVariable& aRate);

   //! Mutators for member-specific rate containers
   //! Returns true if add/remove operation successful
   //! Presence of optional simulation pointer indicates
   //! the need to schedule the event.
   //@{
   void AddNamedRate(const NamedComm& aComm, const WsfRandomVariable& aRate, WsfSimulation* aSimPtr = nullptr);
   void AddAddressRate(const Address& aAddress, const WsfRandomVariable& aRate, WsfSimulation* aSimPtr = nullptr);
   bool RemoveNamedRate(const NamedComm& aComm);
   bool RemoveAddressRate(const Address& aAddress);
   //@}

   //! Find methods for specified member rate
   //! Returns nullptr if rate is not found
   //@{
   WsfRandomVariable* GetNamedRate(const NamedComm& aComm);
   WsfRandomVariable* GetAddressRate(const Address& aAddress);
   //@}

protected:
   //! Input methods for member-specific update rates.
   //@{
   bool ProcessNamedRate(UtInput& aInput);
   bool ProcessAddressRate(UtInput& aInput);
   //@}

   //! Update Methods
   //! These methods drive the connectivity updates for the network.
   //! The Update method with only the sim parameter is called for the
   //! generic update interval, while each member that has a
   //! specified rate uses the appropriate Update call.
   //! These methods are only called from within the class at appropriate
   //! times (via a scheduled event) and will reschedule the event as
   //! necessary.
   //@{
   void Update(WsfSimulation& aSim);
   void Update(WsfSimulation& aSim, const NamedComm& aComm);
   void Update(WsfSimulation& aSim, const Address& aAddress);
   //@}

private:
   //! Generic update rate. Defaults to max double value, so default will never fire during simulation unless
   //! user provides a different rate.
   WsfRandomVariable mUpdateRate{WsfRandomVariable::cCONSTANT,
                                 std::numeric_limits<double>::max(),
                                 0.0,
                                 WsfRandomVariable::cPOSITIVE};

   //! Member based update rate specification.
   //! Note that specifications for members not in the network is allowed, so
   //! that any potential future additions to the network have an established rate
   //! to use if the user does not want the generic rate to apply to that comm.
   NamedRateMap   mNamedRate{};
   AddressRateMap mAddressRate{};

   //! For wsf::comm::GenericEvent
   std::shared_ptr<int> mEvents{std::make_shared<int>(0)};
};

class WSF_EXPORT ScriptNetworkAdHocClass : public ScriptNetworkGenericClass
{
public:
   ScriptNetworkAdHocClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;

   UT_DECLARE_SCRIPT_METHOD(ChangeUpdateRate);
   UT_DECLARE_SCRIPT_METHOD(AddMemberUpdateRate);
   UT_DECLARE_SCRIPT_METHOD(RemoveMemberUpdateRate);
};

} // namespace comm
} // namespace wsf

#endif
