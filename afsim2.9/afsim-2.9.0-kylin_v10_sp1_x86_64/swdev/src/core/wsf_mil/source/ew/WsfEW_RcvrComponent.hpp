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

#ifndef WSFEW_RCVRCOMPONENT_HPP
#define WSFEW_RCVRCOMPONENT_HPP

#include "WsfEM_Rcvr.hpp"
class WsfEW_EA_EP;
#include "wsf_mil_export.h"

#include "WsfMilComponentRoles.hpp"

class WSF_MIL_EXPORT WsfEW_RcvrComponent : public WsfEM_RcvrComponent
{
public:
   static WsfEW_RcvrComponent* Find(const WsfEM_Rcvr& aRcvr);
   static WsfEW_RcvrComponent* FindOrCreate(WsfEM_Rcvr& aRcvr);

   WsfEW_RcvrComponent();
   WsfEW_RcvrComponent(const WsfEW_RcvrComponent& aSrc);
   WsfEW_RcvrComponent& operator=(const WsfEW_RcvrComponent&) = delete;
   ~WsfEW_RcvrComponent() override;

   //! @name WsfComponent required methods.
   //@{
   WsfStringId   GetComponentName() const override;
   WsfComponent* CloneComponent() const override;
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! @name WsfComponent optional methods.
   //@{
   bool Initialize(double aSimTime) override;
   //@}

   //! @name WsfEM_RcvrComponent methods.
   //@{
   void RemoveInteractor(WsfEM_Xmtr* aXmtrPtr) override;
   //@}

   //! @name EW-specific methods
   //@{
   WsfEW_EA_EP* GetElectronicProtect() const { return mElectronicProtectPtr; }
   void         SetElectronicProtect(WsfEW_EA_EP* aElectronicProtectPtr);
   //@}

private:
   WsfEW_EA_EP* mElectronicProtectPtr;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfEW_RcvrComponent, cWSF_COMPONENT_EW_RCVR)

#endif
