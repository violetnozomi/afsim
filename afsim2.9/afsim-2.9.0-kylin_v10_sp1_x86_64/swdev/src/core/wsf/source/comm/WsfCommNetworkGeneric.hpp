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

#ifndef WSFCOMMNETWORKGENERIC_HPP
#define WSFCOMMNETWORKGENERIC_HPP

#include "WsfCommNetwork.hpp"

namespace wsf
{
namespace comm
{

//! A generic network implementation, using user defined links
//! without any specific topology.
class WSF_EXPORT NetworkGeneric : public Network
{
public:
   NetworkGeneric() = default;
   NetworkGeneric(const std::string& aNetworkName, const Address& aAddress);
   ~NetworkGeneric() override                 = default;
   NetworkGeneric(const NetworkGeneric& aSrc) = default;
   NetworkGeneric& operator=(const NetworkGeneric& aRhs) = default;
   NetworkGeneric(NetworkGeneric&& aRhs)                 = default;

   bool            ProcessInput(UtInput& aInput) override;
   NetworkGeneric* Clone() const override;
   const char*     GetScriptClassName() const override { return "WsfNetworkGeneric"; }

protected:
   bool InitializeLinkageP(WsfSimulation& aSimulation) override;
   bool AddLinkP(const Address& aSourceAddress,
                 const Address& aDestinationAddress,
                 WsfSimulation& aSimulation,
                 const Address& aOptionalAddress = Address()) override;
   bool RemoveLinkP(const Address& aSourceAddress,
                    const Address& aDestinationAddress,
                    WsfSimulation& aSimulation,
                    const Address& aOptionalAddress = Address()) override;
   bool RemoveMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
};

class WSF_EXPORT ScriptNetworkGenericClass : public ScriptNetworkClass
{
public:
   //! Note: The address value is optional. It should be automatically assigned
   //! if not provided.
   ScriptNetworkGenericClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;
};

} // namespace comm
} // namespace wsf

#endif
