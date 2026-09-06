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

#ifndef WSFCOMMNETWORKSTAR_HPP
#define WSFCOMMNETWORKSTAR_HPP

#include "WsfCommNetwork.hpp"

namespace wsf
{
namespace comm
{

//! A network implementation, where all comms have bi-directional
//! linkage to a single member.
class WSF_EXPORT NetworkStar : public Network
{
public:
   NetworkStar() = default;
   NetworkStar(const std::string& aNetworkName, const Address& aAddress);
   ~NetworkStar() override = default;
   NetworkStar& operator=(const NetworkStar& aRhs) = default;

   bool         ProcessInput(UtInput& aInput) override;
   NetworkStar* Clone() const override;
   const char*  GetScriptClassName() const override { return "WsfNetworkStar"; }

   //! @name Add/Remove Hub Methods
   //! These methods are specific to the star topology implementation.
   //! They allow setting or removing of the special hub member.
   //! A hub may only be added if one does not currently exist, and will
   //! result in linkage to all currently defined members.
   //! Removing a hub will remove all linkage.
   //! @note These are for programmatic usage only, or for scripting.
   //! It is expected that typical usage is setting the hub via
   //! ProcessInput().
   //@{
   virtual bool AddHub(const Address& aAddress, WsfSimulation& aSimulation);
   virtual bool RemoveHub(const Address& aAddress, WsfSimulation& aSimulation);
   //@}

protected:
   bool InitializeLinkageP(WsfSimulation& aSimulation) override;
   bool AddMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
   bool RemoveMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;

   NamedComm mHubNamed{};
   Address   mHubAddress{};
};

class WSF_EXPORT ScriptNetworkStarClass : public ScriptNetworkClass
{
public:
   //! Note: The address value is optional. It should be automatically assigned
   //! if not provided.
   ScriptNetworkStarClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptNetworkStarClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;

   UT_DECLARE_SCRIPT_METHOD(AddHub_1);
   UT_DECLARE_SCRIPT_METHOD(AddHub_2);
   UT_DECLARE_SCRIPT_METHOD(RemoveHub_1);
   UT_DECLARE_SCRIPT_METHOD(RemoveHub_2);
};

} // namespace comm
} // namespace wsf

#endif
