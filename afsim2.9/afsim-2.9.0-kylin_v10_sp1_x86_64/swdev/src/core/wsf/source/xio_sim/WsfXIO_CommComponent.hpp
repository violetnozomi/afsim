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

#ifndef WSFXIO_COMMCOMPONENT_HPP
#define WSFXIO_COMMCOMPONENT_HPP

#include "WsfCommComponent.hpp"

class WsfXIO_AutoDisMapping;
class WsfXIO_Connection;

namespace wsf
{
namespace xio
{

//! The hardware component that can be attached to all comm systems.
class CommComponent : public wsf::comm::Component
{
public:
   static CommComponent* Find(const wsf::comm::Comm& aParent);
   static CommComponent* FindOrCreate(wsf::comm::Comm&       aParent,
                                      WsfXIO_AutoDisMapping* aDisMappingPtr,
                                      WsfXIO_Connection*     aConnectionPtr);

   CommComponent(WsfXIO_AutoDisMapping* aDisMappingPtr, WsfXIO_Connection* aConnectionPtr);
   CommComponent(const CommComponent& aSrc) = default;
   ~CommComponent() override                = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfCommComponent.
   //@{
   bool Receive(double aSimTime, wsf::comm::Comm* aXmtrPtr, wsf::comm::Message& aMessage) override;
   //@}

private:
   WsfXIO_AutoDisMapping* mDisMappingPtr;
   WsfXIO_Connection*     mConnectionPtr;
};

} // namespace xio
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::xio::CommComponent, cWSF_COMPONENT_COMM_XIO)

#endif
