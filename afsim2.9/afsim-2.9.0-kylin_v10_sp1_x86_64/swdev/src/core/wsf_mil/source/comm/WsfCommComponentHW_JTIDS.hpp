// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMCOMPONENTHWJTIDS_HPP
#define WSFCOMMCOMPONENTHWJTIDS_HPP

#include "wsf_mil_export.h"

#include <memory>

#include "WsfCommComponentHW.hpp"
#include "WsfMilComponentRoles.hpp"

// Forward declarations
class WsfCommResult;

namespace wsf
{
namespace comm
{

//! The hardware component that can be attached to all comm systems.
class WSF_MIL_EXPORT ComponentHW_JTIDS : public ComponentHW
{
public:
   static ComponentHW_JTIDS* Find(const Comm& aParent);
   static ComponentHW_JTIDS* FindOrCreate(Comm& aParent);

   ComponentHW_JTIDS();
   ~ComponentHW_JTIDS() override = default;

   //! @name Required interface from WsfComponent.
   //@{
   WsfComponent* CloneComponent() const override;
   WsfStringId   GetComponentName() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Interface from WsfCommComponent.
   //@{
   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   //@}

   bool CanReceiveFrom(double aSimTime, Comm* aXmtrPtr, Result& aResult) override;

protected:
   ComponentHW_JTIDS(const ComponentHW_JTIDS& aSrc) = default;
   ComponentHW_JTIDS& operator=(const ComponentHW_JTIDS& aRhs) = delete;

private:
   // The maximum range between the receiver and transmitter.
   double mMaximumRange;
};

} // namespace comm
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::comm::ComponentHW_JTIDS, cWSF_COMPONENT_COMM_HW_JTIDS)

#endif
