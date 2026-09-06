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

#ifndef WSFCOMMNETWORKP2P_HPP
#define WSFCOMMNETWORKP2P_HPP

#include "WsfCommNetwork.hpp"

namespace wsf
{
namespace comm
{

//! A point-to-point network implementation, defined as only
//! two comms with bi-directional linkage.
class WSF_EXPORT NetworkPointToPoint : public Network
{
public:
   NetworkPointToPoint() = default;
   NetworkPointToPoint(const std::string& aNetworkName, const Address& aAddress);
   ~NetworkPointToPoint() override = default;
   NetworkPointToPoint& operator=(const NetworkPointToPoint& aRhs) = default;

   bool                 ProcessInput(UtInput& aInput) override;
   NetworkPointToPoint* Clone() const override;
   const char*          GetScriptClassName() const override { return "WsfNetworkP2P"; }

protected:
   bool InitializeLinkageP(WsfSimulation& aSimulation) override;
   bool AddMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
   bool RemoveMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
};

class WSF_EXPORT ScriptNetworkPointToPointClass : public ScriptNetworkClass
{
public:
   //! Note: The address value is optional. It should be automatically assigned
   //! if not provided.
   ScriptNetworkPointToPointClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptNetworkPointToPointClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;
};

} // namespace comm
} // namespace wsf

#endif
