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

#ifndef WSFCOMMNETWORKRING_HPP
#define WSFCOMMNETWORKRING_HPP

#include "WsfCommNetwork.hpp"

namespace wsf
{
namespace comm
{

//! A network implementation, where all members have bi-directional
//! linkage to two other unique members in the network.
//! @note If at least three comms are not defined by the user, this
//! network will not establish its links.
class WSF_EXPORT NetworkRing : public Network
{
public:
   NetworkRing() = default;
   NetworkRing(const std::string& aNetworkName, const Address& aAddress);
   ~NetworkRing() override = default;
   NetworkRing& operator=(const NetworkRing& aRhs) = default;

   bool         ProcessInput(UtInput& aInput) override;
   NetworkRing* Clone() const override;
   const char*  GetScriptClassName() const override { return "WsfNetworkRing"; }

protected:
   //! Linkage initialized by this implementation will order the members of the ring
   //! by the order in which they were processed via ProcessInput(). Thus, the order
   //! in which the user lists members matters in this case. For programmatic or script
   //! method control of addition or removal at specific locations, see the
   //! Add/RemoveMemberRing methods.
   bool InitializeLinkageP(WsfSimulation& aSimulation) override;

   bool AddMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
   bool RemoveMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
};

//! The same functionality as NetworkRing, except each member only has
//! one outgoing and one incoming connection.
class WSF_EXPORT NetworkDirectedRing : public NetworkRing
{
public:
   NetworkDirectedRing() = default;
   NetworkDirectedRing(const std::string& aNetworkName, const Address& aAddress);
   ~NetworkDirectedRing() override = default;
   NetworkDirectedRing& operator=(const NetworkDirectedRing& aRhs) = default;

   NetworkDirectedRing* Clone() const override;
   const char*          GetScriptClassName() const override { return "WsfNetworkDirectedRing"; }

protected:
   bool InitializeLinkageP(WsfSimulation& aSimulation) override;
   bool AddMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
   bool RemoveMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
};

class WSF_EXPORT ScriptNetworkRingClass : public ScriptNetworkClass
{
public:
   //! Note: The address value is optional. It should be automatically assigned
   //! if not provided.
   ScriptNetworkRingClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptNetworkRingClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;
};

class WSF_EXPORT ScriptNetworkDirectedRingClass : public ScriptNetworkRingClass
{
public:
   ScriptNetworkDirectedRingClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;
};

} // namespace comm
} // namespace wsf

#endif
