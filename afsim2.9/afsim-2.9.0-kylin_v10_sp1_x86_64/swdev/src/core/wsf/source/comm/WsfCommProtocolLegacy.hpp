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

#ifndef WSFCOMMPROTOCOLLEGACY_HPP
#define WSFCOMMPROTOCOLLEGACY_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"
#include "WsfCommGraph.hpp"
#include "WsfCommProtocolPriority.hpp"
#include "WsfCommRouter.hpp"
#include "WsfCommRouterProtocolInterface.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

class WSF_EXPORT ProtocolLegacy : public ProtocolInterface
{
public:
   static ProtocolLegacy* Find(const Router& aParent);
   static ProtocolLegacy* FindOrCreate(Router& aParent);

   ProtocolLegacy()           = default;
   ~ProtocolLegacy() override = default;

   //! @name Required ProtocolInterface interface.
   //@{
   WsfComponent*   CloneComponent() const override;
   const int*      GetComponentRoles() const override;
   void*           QueryInterface(int aRole) override;
   int             GetPriority() const override;
   ProtocolLegacy* Clone() const override;

   bool Send(double aSimTime, Router::SendData& aData) override;

   std::vector<Address> Routing(double         aSimTime,
                                const Address& aSendingInterface,
                                size_t&        aHopNumber,
                                double&        aCost,
                                const Message& aMessage) override;

   const char* GetScriptClassName() const override { return "WsfCommProtocolLegacy"; }
   //@}

   void Setup() override;

   //! @name Graph methods
   //! This protocol uses truth, via the network manager, at all times.
   //! We don't propagate the truth, because we aren't keeping a local
   //! copy and using the network manager's graph directly. We don't
   //! callbacks acting on this graph, because this graph is the same
   //! one pushing the notifications, and has already been updated.
   //@{
   bool          ShouldPropagateTruth() const override { return false; }
   graph::Graph* GetGraph() const override { return mGraph; }
   //@}

   bool Initialize(double aSimTime) override;

private:
   graph::Graph* mGraph{nullptr};
};

//! Script class for this protocol.
class WSF_EXPORT ScriptProtocolClassLegacy : public ScriptRouterProtocolClass
{
public:
   ScriptProtocolClassLegacy(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptProtocolClassLegacy() override = default;
};

} // namespace router
} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::router::ProtocolLegacy, cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_LEGACY);

#endif
