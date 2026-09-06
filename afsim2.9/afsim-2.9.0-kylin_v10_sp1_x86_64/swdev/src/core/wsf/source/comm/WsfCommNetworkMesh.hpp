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

#ifndef WSFCOMMNETWORKMESH_HPP
#define WSFCOMMNETWORKMESH_HPP

#include "UtScriptBasicTypes.hpp"
#include "WsfCommNetwork.hpp"

namespace wsf
{
namespace comm
{

//! A network implementation, defined as any number of comms
//! with bi-directional linkage between all members.
class WSF_EXPORT NetworkMesh : public Network
{
public:
   NetworkMesh() = default;
   NetworkMesh(const std::string& aNetworkName, const Address& aAddress);
   ~NetworkMesh() override = default;
   NetworkMesh& operator=(const NetworkMesh& aRhs) = default;

   bool         ProcessInput(UtInput& aInput) override;
   NetworkMesh* Clone() const override;
   const char*  GetScriptClassName() const override { return "WsfNetworkMesh"; }

protected:
   bool InitializeLinkageP(WsfSimulation& aSimulation) override;
   bool AddMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
   bool RemoveMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
};

//! A network implementation, defined as any number of comms
//! with bi-directional linkage between all members. Linkage in this version is
//! dependent on the members being able to "talk" to each other. This was the
//! default network usage in legacy comms.
class WSF_EXPORT NetworkMeshLegacy : public NetworkMesh
{
public:
   NetworkMeshLegacy() = default;
   NetworkMeshLegacy(const std::string& aNetworkName, const Address& aAddress);
   ~NetworkMeshLegacy() override = default;
   NetworkMeshLegacy& operator=(const NetworkMeshLegacy& aRhs) = default;

   NetworkMeshLegacy* Clone() const override;
   const char*        GetScriptClassName() const override { return "WsfNetworkMeshLegacy"; }

protected:
   bool AddMemberP(const Address& aAddress, WsfSimulation& aSimulation, const Address& aOptionalAddress = Address()) override;
};

class WSF_EXPORT ScriptNetworkMeshClass : public ScriptNetworkClass
{
public:
   //! Note: The address value is optional. It should be automatically assigned
   //! if not provided.
   ScriptNetworkMeshClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptNetworkMeshClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;
};

class WSF_EXPORT ScriptNetworkMeshLegacyClass : public ScriptNetworkMeshClass
{
public:
   ScriptNetworkMeshLegacyClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aNetworkPtr) override;
};

} // namespace comm
} // namespace wsf

#endif
