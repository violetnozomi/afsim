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

#ifndef WSFEW_XMTRCOMPONENT_HPP
#define WSFEW_XMTRCOMPONENT_HPP

#include "WsfEM_Xmtr.hpp"
class WsfEW_EA_EP;
#include "wsf_mil_export.h"

#include "WsfMilComponentRoles.hpp"

class WSF_MIL_EXPORT WsfEW_XmtrComponent : public WsfEM_XmtrComponent
{
public:
   static WsfEW_XmtrComponent* Find(const WsfEM_Xmtr& aXmtr);
   static WsfEW_XmtrComponent* FindOrCreate(WsfEM_Xmtr& aXmtr);

   WsfEW_XmtrComponent();
   WsfEW_XmtrComponent(const WsfEW_XmtrComponent& aSrc);
   WsfEW_XmtrComponent& operator=(const WsfEW_XmtrComponent&) = delete;
   ~WsfEW_XmtrComponent() override;

   //! @name Component infrastructure methods.
   //@{
   WsfStringId   GetComponentName() const override;
   WsfComponent* CloneComponent() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name Common infrastructure methods.
   //@{
   bool Initialize(double aSimTime) override;
   //@}

   //! EW-specific transmitter methods
   //@{
   WsfEW_EA_EP* GetElectronicAttack() const { return mElectronicAttackPtr; }
   void         SetElectronicAttack(WsfEW_EA_EP* aElectronicAttackPtr);
   //@}

private:
   WsfEW_EA_EP* mElectronicAttackPtr;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfEW_XmtrComponent, cWSF_COMPONENT_EW_XMTR)

#endif
