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

#ifndef WSFCOMMANDCHAIN_HPP
#define WSFCOMMANDCHAIN_HPP

#include "wsf_export.h"

#include <memory>
#include <string>
#include <vector>

class UtInput;
class UtScriptClass;
class UtScriptTypes;
class WsfPlatform;
class WsfScenario;
#include "WsfSimpleComponent.hpp"
#include "WsfStringId.hpp"

//! The command and control reporting structure of a platform.
//!
//! This object indicates the immediate commander, peers and
//! of a specific platform within a specific command chain.
//!
//! @note A platform may be part of more than one command chain.
class WSF_EXPORT WsfCommandChain : public WsfSimplePlatformComponent
{
public:
   typedef std::vector<WsfPlatform*>           PlatformList;
   typedef std::vector<WsfPlatform*>::iterator PlatformListIter;

   static void RegisterComponentFactory(WsfScenario& aScenario);

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static std::string GetDefaultName();
   static WsfStringId GetDefaultNameId();

   WsfCommandChain();
   WsfCommandChain(WsfStringId aCommandChainName, WsfStringId aCommanderName);
   ~WsfCommandChain() override = default;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return new WsfCommandChain(*this); }
   void*         QueryInterface(int aRole) override { return (aRole == mRoles[0]) ? this : nullptr; }
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_COMMAND_CHAIN; }
   void          ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }
   //@}

   //! Get the platform associated with this node in the command chain.
   //! @returns The platform with which this node in the command chain is associated.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! @name Commander methods.
   //@{
   //! Get the commander of the command chain
   //! @returns The commander (WsfPlatform*) of the command chain.
   WsfPlatform* GetCommander() const { return mCommanderPtr; }

   void SetCommander(WsfPlatform* aPlatformPtr);

   //! Get the name of the commander.
   std::string GetCommanderName() const;

   //! Get the name ID of the commander;
   WsfStringId GetCommanderNameId() const { return mCommanderName; }
   //@}

   //! @name Peer methods.
   //@{
   //! Get the list of peers in the command chain
   //! @returns The list of peers (WsfCommandChain::PlatformList) in the command chain.
   PlatformList& GetPeers() { return mPeers; }

   void AddPeer(WsfPlatform* aPlatformPtr);
   void RemovePeer(WsfPlatform* aPlatformPtr);
   //@}

   //! @name Subordinate methods.
   //@{
   //! Get the list of subordinates in the command chain
   //! @returns The list of subordinates (WsfCommandChain::PlatformList) in the command chain.
   PlatformList& GetSubordinates() { return mSubordinates; }

   void AddSubordinate(WsfPlatform* aPlatformPtr);
   void RemoveSubordinate(WsfPlatform* aPlatformPtr);
   //@}

   //! @name Simulation infrastructure methods.
   //@(
   bool Initialize(double aSimTime) override;

   static bool LoadInstance(UtInput& aInput, WsfCommandChain*& aInstancePtr);

   void PlatformAdded(WsfPlatform* aPlatformPtr);
   void PlatformDeleted(WsfPlatform* aPlatformPtr);
   //@}

protected:
   WsfCommandChain(const WsfCommandChain& aSrc);
   WsfCommandChain& operator=(const WsfCommandChain& aRhs);

private:
   std::string PrintHeader() const;

   WsfPlatform* mPlatformPtr;

   // This flag indicates that a commander was not specified for this command chain and it assumed to be SELF
   // This should only happen for the default command chain due to AFSIM's input restrictions.
   bool                      mCommanderIsSelfImplicitly{false};
   WsfStringId               mCommanderName;
   WsfPlatform*              mCommanderPtr;
   std::vector<WsfPlatform*> mPeers;
   std::vector<WsfPlatform*> mSubordinates;

   UtScriptClass* mPlatformClassPtr;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfCommandChain, cWSF_COMPONENT_COMMAND_CHAIN)

#endif
