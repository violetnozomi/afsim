// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFEW_COMMCOMPONENT_HPP
#define WSFEW_COMMCOMPONENT_HPP

#include "wsf_mil_export.h"

#include "WsfCommComponent.hpp"
#include "WsfMilComponentRoles.hpp"
class WsfScenario;

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

//! The EW component that will be attached to all comm systems.
class WSF_MIL_EXPORT WsfEW_CommComponent : public wsf::comm::Component
{
public:
   static void                 RegisterComponentFactory(WsfScenario& aScenario);
   static WsfEW_CommComponent* Find(const wsf::comm::Comm& aParent);
   static WsfEW_CommComponent* FindOrCreate(wsf::comm::Comm& aParent);

   ~WsfEW_CommComponent() override = default;

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
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void AttemptToReceive(double aSimTime, wsf::comm::Result& aResult) override;
   //@}

   //! @name WsfEW_CommComponent methods.
   //@{
   bool   CanPerceiveJamming() const;
   bool   JammingPerceived();
   double GetJammingFirstPerceived() const { return mJammingFirstPerceived; }
   double GetJammingLastPerceived() const { return mJammingLastPerceived; }
   //@}

private:
   bool PerceivesJamming(double aSimTime, wsf::comm::Result& aResult);

   double mJammingFirstPerceived      = -1.0;
   double mJammingLastPerceived       = -1.0;
   double mJammingPerceptionResetTime = 0.0;

   //! The required JNR for continuous type jamming perception by the operator.
   double mContJNR_PerceptionThreshold = 1.0E38;

   //! The required JNR for pulse type jamming perception by the operator.
   double mPulsedJNR_PerceptionThreshold = 1.0E38;

   //! The required JNR for coherent (FT) jamming perception by the operator.
   double mCoherentJNR_PerceptionThreshold = 1.0E38;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfEW_CommComponent, cWSF_COMPONENT_EW_COMM)

#endif
