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

#ifndef WSFCOMMPROTOCOLADHOC_HPP
#define WSFCOMMPROTOCOLADHOC_HPP

#include "wsf_export.h"

#include "WsfCommRouterProtocolInterface.hpp"
#include "WsfRandomVariable.hpp"
class WsfScenario;
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

//! Class ProtocolAdHoc provides a router protocol for usage in generic
//! ad-hoc routing, or simply for generic script based routing protocol
//! capabilities.
//!
//! In general, ad-hoc routing protocols vary greatly and often are highly
//! customizable in the rules used to determine routing and connectivity.
//! Concerns exist that often aren't applicable in other use cases, such
//! as power consumption, network congestion, or unique security concerns.
//! As such, this protocol provides the ability to the user to directly define
//! these rules, to the extent that they are needed or can be modeled within AFSIM
//! using the scripting language. This provides a highly customizable
//! router protocol with basic infrastructure, such that each can be customized
//! specific to the analysis and use case. This also allows users to modify the protocol
//! behavior to the maximum extent possible without requiring code based
//! development or changes where general behaviors are sufficient and a high
//! level of fidelity is not required, as typical in the common AFSIM mission
//! level simulation usage.
//!
//! Several pre-defined methods are associated with this routing protocol
//! for definition by the user. Definition of these script methods allow
//! the user to dictate the protocol behavior in these instances.
//!
//! While most ad-hoc protocols do not require routers in the traditional sense,
//! they do provide routing capabilities. As such, this protocol is provided
//! as a router protocol.
//! The definitions associated with this protocol define every interface associated
//! with the router, which may not be intended behavior. In such cases, users
//! may simply associate routers with comm interfaces on a one-to-one basis.
class ProtocolAdHoc : public ProtocolInterface
{
public:
   static ProtocolAdHoc* Find(const Router& aParent);
   static ProtocolAdHoc* FindOrCreate(Router& aParent);

   ProtocolAdHoc(const WsfScenario& aScenario);
   ProtocolAdHoc(const ProtocolAdHoc& aSrc);
   ~ProtocolAdHoc() override = default;

   //! @name Required ProtocolInterface interface.
   //@{
   WsfComponent*  CloneComponent() const override;
   const int*     GetComponentRoles() const override;
   void*          QueryInterface(int aRole) override;
   int            GetPriority() const override;
   ProtocolAdHoc* Clone() const override;

   bool Send(double aSimTime, Router::SendData& aData) override;

   std::vector<Address> Routing(double         aSimTime,
                                const Address& aSendingInterface,
                                size_t&        aHopNumber,
                                double&        aCost,
                                const Message& aMessage) override;

   const char* GetScriptClassName() const override { return "WsfCommProtocolAdHoc"; }
   //@}

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Setup() override;

   //! @name Graph methods
   //@{
   bool          ShouldPropagateTruth() const override { return false; }
   graph::Graph* GetGraph() const override { return mGraph.get(); }
   //@}

   //! @name Script method overload execution methods
   //! These methods execute the user defined script
   //! corresponding with the network state notification.
   //@{
   void ExecuteOnCommAdded(double aSimTime, const Address& aComm);
   void ExecuteOnCommRemoved(double aSimTime, const Address& aComm);
   void ExecuteOnConnectionAdded(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress);
   void ExecuteOnConnectionRemoved(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress);
   Address ExecuteOnMessageRouting(double aSimTime, const Address& aInterface, const Message& aMessage);
   //@}

   //! @name Router push notifications
   //! These methods, from the ProtocolInterface, are called
   //! by the router when truth based network state data has
   //! been indicated to the router via the network manager.
   //! These methods will schedule any potential delays/events
   //! for the corresponding knowledge to propagate for this protocol.
   //! @note These are the only actions supported at this time.
   //@{
   void PerfectCommAdded(double aSimTime, Comm* aCommPtr) override;
   void PerfectCommRemoved(double aSimTime, Comm* aCommPtr) override;
   void PerfectConnectionAdded(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress) override;
   void PerfectConnectionRemoved(double aSimTime, const Address& aSourceAddress, const Address& aDestinationAddress) override;
   //@}

   //! A helper method to filter incoming updates. In this implementation, the only
   //! changes to network state that are tracked are those for interfaces that share
   //! a network with any of the interfaces on this router. Returns true if the
   //! address is one that we currently track.
   bool ValidNetwork(const Address& aAddress) const;

   //! A helper function for initialization. This copies truth state at simulation
   //! start to this protocol.
   void InitializeState();

private:
   const WsfScenario&                mScenario;
   std::unique_ptr<graph::Graph>     mGraph{nullptr};
   std::unique_ptr<WsfScriptContext> mContextPtr{nullptr};

   WsfRandomVariable mOnCommAddedDelay{WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};
   WsfRandomVariable mOnCommRemovedDelay{WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};
   WsfRandomVariable mOnConnectionAddedDelay{WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};
   WsfRandomVariable mOnConnectionRemovedDelay{WsfRandomVariable::cCONSTANT, 0.0, 0.0, WsfRandomVariable::cNON_NEGATIVE};

   UtScript* mOnCommAdded{nullptr};
   UtScript* mOnCommRemoved{nullptr};
   UtScript* mOnConnectionAdded{nullptr};
   UtScript* mOnConnectionRemoved{nullptr};
   UtScript* mOnMessageRouting{nullptr};

   std::shared_ptr<int> mEvents{std::make_shared<int>(0)};
};

} // namespace router
} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::router::ProtocolAdHoc, cWSF_COMPONENT_COMM_ROUTER_PROTOCOL_AD_HOC);

#endif
