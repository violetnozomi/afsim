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

#include "WsfCommandChain.hpp"

#include "wsf_export.h"

#include <algorithm>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"
#include "UtScriptTypes.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptManager.hpp"

// Notes on the 'default' command chain.
//
// Each platform contains a 'default' command chain. This chain can be populated by using the command
// 'commander <cmdr-name>', but it also implicitly populated using the any named command chains for
// which the platform is a part. Thus the subordinate and peer list of the default command chain represents
// the subordinates and peers respectively for all command chains of which the platform is a part.
//
// Also, if the default command chain does not have an explicitly defined commander then the commander
// on the default chain will be the commander of the first named command chain.
//
// ATTENTION: The behavior stated above will be removed in a future release. This old behavior leads to
// ambiguity for the user when multiple explicitly named command chains exist. The future behavior may
// either remove the default command chain or implicitly set a platform's commander as itself in the default
// command chain unless another platform is explicitly named.
// (See AFSIM-858)

//#define DEBUG_ON

namespace
{
//! Component factory to process platform input.
class CommandChainComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessInput(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfCommandChain* chainPtr(nullptr);
      bool             myCommand(false);
      if (WsfCommandChain::LoadInstance(aInput, chainPtr))
      {
         myCommand = true;
         aPlatform.DeleteComponent<WsfCommandChain>(chainPtr->GetNameId());
         if (!aPlatform.AddComponent(chainPtr)) // Failure should never happen
         {
            throw UtInput::BadValue(aInput, "Unexpected error adding command_chain");
         }
      }
      return myCommand;
   }

   // Implements WsfComponentFactor::PreInitialize (called from WsfPlatform)
   bool PreInitialize(double /* aSimTime */, WsfPlatform& aPlatform) override
   {
      // Create the default command chain if it doesn't already exist.
      if (aPlatform.GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId()) == nullptr)
      {
         aPlatform.AddComponent(new WsfCommandChain(WsfCommandChain::GetDefaultNameId(), WsfStringId()));
      }
      return true;
   }
};

} // namespace

// static
void WsfCommandChain::RegisterComponentFactory(WsfScenario& aScenario)
{
   aScenario.RegisterComponentFactory(ut::make_unique<CommandChainComponentFactory>());
}

//! Get the name to be used for the default command chain.
// static
std::string WsfCommandChain::GetDefaultName()
{
   return std::string("default");
}

//! Get the name (string ID) to be used for the default command chain.
// static
WsfStringId WsfCommandChain::GetDefaultNameId()
{
   return UtStringIdLiteral("default");
}

// ================================================================================================
WsfCommandChain::WsfCommandChain()
   : WsfSimplePlatformComponent(cWSF_COMPONENT_COMMAND_CHAIN)
   , mPlatformPtr(nullptr)
   , mCommanderName(nullptr)
   , mCommanderPtr(nullptr)
   , mPeers()
   , mSubordinates()
   , mPlatformClassPtr(nullptr)
{
}

// ================================================================================================
//! Constructor with chain name and commander.
WsfCommandChain::WsfCommandChain(WsfStringId aCommandChainName, WsfStringId aCommanderName)
   : WsfSimplePlatformComponent(cWSF_COMPONENT_COMMAND_CHAIN, aCommandChainName)
   , mPlatformPtr(nullptr)
   , mCommanderName(aCommanderName)
   , mCommanderPtr(nullptr)
   , mPeers()
   , mSubordinates()
   , mPlatformClassPtr(nullptr)
{
}

// ================================================================================================
// protected
//! Copy constructor (for Clone())
WsfCommandChain::WsfCommandChain(const WsfCommandChain& aSrc)
   : WsfSimplePlatformComponent(aSrc)
   , mPlatformPtr(nullptr)
   , mCommanderName(aSrc.mCommanderName)
   , mCommanderPtr(aSrc.mCommanderPtr)
   , mPeers()
   , mSubordinates()
   , mPlatformClassPtr(aSrc.mPlatformClassPtr)
{
}

WsfCommandChain& WsfCommandChain::operator=(const WsfCommandChain& aRhs)
{
   if (this != &aRhs)
   {
      WsfSimplePlatformComponent::operator=(aRhs);
      mPlatformPtr                        = nullptr;
      mCommanderName                      = aRhs.mCommanderName;
      mCommanderPtr                       = aRhs.mCommanderPtr;
      mPeers.clear();
      mSubordinates.clear();
      mPlatformClassPtr = aRhs.mPlatformClassPtr;
   }
   return *this;
}

// ================================================================================================
//! Define a new commander for this command chain.
//! @param aPlatformPtr The new commander.
//! @note This assumes that the new command defined by the input argument exists AND has been initialized.
void WsfCommandChain::SetCommander(WsfPlatform* aPlatformPtr)
{
   mCommanderIsSelfImplicitly = false;

   if (aPlatformPtr == nullptr)
   {
      return;
   }

   if (mCommanderName == aPlatformPtr->GetNameId())
   {
      // Commander didn't change, ensure the commander pointer is set properly and exit if not set or valid
      if ((mCommanderPtr == nullptr) || (mCommanderPtr == aPlatformPtr))
      {
         mCommanderPtr = aPlatformPtr;
         return;
      }
   }

   WsfCommandChain* chainPtr;

   // clear me from my previous commander's subordinate list
   if (mCommanderPtr != nullptr)
   {
      chainPtr = mCommanderPtr->GetComponent<WsfCommandChain>(GetNameId());
      if (chainPtr != nullptr)
      {
         chainPtr->RemoveSubordinate(mPlatformPtr);
      }
   }

   // Clear me from my previous peers' peer lists (and remove them from my own peer list)
   for (WsfPlatform* peerPtr : mPeers)
   {
      chainPtr = peerPtr->GetComponent<WsfCommandChain>(GetNameId());
      if (chainPtr != nullptr)
      {
         chainPtr->RemovePeer(mPlatformPtr);
      }
   }
   mPeers.clear();

   // Setup my new commander
   mCommanderPtr  = aPlatformPtr;
   mCommanderName = mCommanderPtr->GetNameId();

   // Clear my new commander from my previous peers' peer lists, if present
   RemovePeer(mCommanderPtr);

   chainPtr = mCommanderPtr->GetComponent<WsfCommandChain>(GetNameId());
   if (chainPtr == nullptr)
   {
      // The new commander doesn't already have this command chain, it's new to him.
      // Give him a command chain with this ID if & make him the "self" commander
      chainPtr = new WsfCommandChain(GetNameId(), mCommanderName);
      mCommanderPtr->AddComponent(chainPtr);
      chainPtr->Initialize(0.0);
   }
   // Set me in my new commander's subordinate list
   chainPtr->AddSubordinate(mPlatformPtr);

   // setup new peer lists
   PlatformList newPeers = chainPtr->GetSubordinates();
   for (WsfPlatform* peerPtr : newPeers)
   {
      if (peerPtr != mPlatformPtr) // Can't be my own peer
      {
         // add my new peer into my peer list
         AddPeer(peerPtr);

         // add me to my new peer's peer list
         chainPtr = peerPtr->GetComponent<WsfCommandChain>(GetNameId());
         if (chainPtr != nullptr)
         {
            chainPtr->AddPeer(mPlatformPtr);
         }
      }
   }
}

// ================================================================================================
//! Get the name of the commander.
std::string WsfCommandChain::GetCommanderName() const
{
   return mCommanderName.GetString();
}

// ================================================================================================
//! Adds the given platform to the list of peers in the command chain
void WsfCommandChain::AddPeer(WsfPlatform* aPlatformPtr)
{
   if (std::find(mPeers.begin(), mPeers.end(), aPlatformPtr) == mPeers.end())
   {
      mPeers.push_back(aPlatformPtr);
   }
}

// ================================================================================================
//! Removes the given platform from the list of peers in the command chain
void WsfCommandChain::RemovePeer(WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      PlatformList::iterator iter;
      if ((iter = std::find(mPeers.begin(), mPeers.end(), aPlatformPtr)) != mPeers.end())
      {
         mPeers.erase(iter);
      }
   }
}

// ================================================================================================
//! Adds the given platform to the list of subordinates in the command chain
void WsfCommandChain::AddSubordinate(WsfPlatform* aPlatformPtr)
{
   if (std::find(mSubordinates.begin(), mSubordinates.end(), aPlatformPtr) == mSubordinates.end())
   {
      mSubordinates.push_back(aPlatformPtr);
   }
}

// ================================================================================================
//! Removes the given platform from the list of subordinates in the command chain
void WsfCommandChain::RemoveSubordinate(WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      PlatformList::iterator iter;
      if ((iter = std::find(mSubordinates.begin(), mSubordinates.end(), aPlatformPtr)) != mSubordinates.end())
      {
         mSubordinates.erase(iter);
      }
   }
}

// ================================================================================================
// Implementation of WsfComponent::Initialize.
bool WsfCommandChain::Initialize(double /* aSimTime */)
{
   bool ok = true;

   // Determine if this platform is its own commander.
   // A platform is its own commander if any of the following are true:
   //
   // - The specified commander name is the same as the platform name
   // - The commander name was specified as SELF
   // - No commander was specified.

   bool assumeCommand = false;
   if (mPlatformPtr->GetNameId() == mCommanderName)
   {
      // 'commander <this_platform_name>' or 'command_chain <chain_name> <this_platform_name>'
      assumeCommand = true;
   }
   else if (mCommanderName == "SELF")
   {
      // 'commander SELF' or 'command_chain <chain_name> SELF'
      assumeCommand = true;
   }
   else if (!mCommanderName)
   {
      // No commander was specified. In a simple world then we would just define ourself to be the commander,
      // but multiple-command chain support makes it a little complicated, thus record that the commander was
      // implicitly defined so it can be used in decision logic later on.
      //
      // NOTE: In theory this should only occur for the default chain. If populated from the input file then
      //       a non-default would always have a commander name because the command format requires it. However,
      //       a chain could be created internally that doesn't have one.
      mCommanderIsSelfImplicitly = true;
      assumeCommand              = true;
   }
   if (assumeCommand)
   {
      mCommanderPtr  = mPlatformPtr;
      mCommanderName = mPlatformPtr->GetNameId();
   }

   mPlatformClassPtr = mPlatformPtr->GetSimulation()->GetScenario().GetScriptTypes()->GetClass("WsfPlatform");
   ok &= (mPlatformClassPtr != nullptr);

   return ok;
}

// ================================================================================================
//! Create a command chain with the specified input.
//! @param aInput A generic input.
//! @param aInstancePtr [output] The command chain to be created.
//! @returns 'true' if the command chain was created; 'false' otherwise.
// static
bool WsfCommandChain::LoadInstance(UtInput& aInput, WsfCommandChain*& aInstancePtr)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if ((command == "command_chain") || (command == "commander"))
   {
      // command_chain <chain-name> <commander-name>
      // commander <commander-name>
      //
      // The command:      commander <commander-name>
      // is equivalent to: command_chain default <commander-name>

      myCommand = true;
      WsfStringId chainName(GetDefaultNameId());
      std::string commanderName;
      if (command == "command_chain")
      {
         aInput.ReadValue(chainName);
      }
      aInput.ReadValue(commanderName);
      aInstancePtr = new WsfCommandChain(chainName, commanderName);
   }
   return myCommand;
}

// ================================================================================================
//! Perform initialization associated with a platform's being added to the simulation.
//! @param aPlatformPtr The platform (WsfPlatform*) that was just added.
void WsfCommandChain::PlatformAdded(WsfPlatform* aPlatformPtr)
{
   // If this command chain is not the 'default' command chain then we'll also update the default chain.
   // See notes at the top of the file.

   WsfCommandChain* defChainPtr = nullptr;
   if (GetNameId() != GetDefaultNameId())
   {
      defChainPtr = mPlatformPtr->GetComponent<WsfCommandChain>(GetDefaultNameId());
   }

   if (aPlatformPtr->GetNameId() == mCommanderName)
   {
      // My commander is being added.
      SetCommander(aPlatformPtr);

      // If this isn't the default command chain and if the default chain does not have a defined commander
      // then use this as the defined commander.

      if ((defChainPtr != nullptr) &&
          ((defChainPtr->mCommanderName.IsNull() ||      // Commander not explicitly defined
            (defChainPtr->mCommanderIsSelfImplicitly)))) // Commander is implicitly defined as SELF
      {
         defChainPtr->SetCommander(aPlatformPtr);
      }
   }
   else
   {
      auto chainPtr = aPlatformPtr->GetComponent<WsfCommandChain>(GetNameId());
      if (chainPtr)
      {
         if (chainPtr->mCommanderName == mPlatformPtr->GetNameId())
         {
            // The platform begin added is one of my direct subordinates
            AddSubordinate(aPlatformPtr);

            // Update the default command chain if necessary.
            if (defChainPtr)
            {
               defChainPtr->AddSubordinate(aPlatformPtr);
            }
         }
         else if (chainPtr->mCommanderName == mCommanderName)
         {
            // The platform being added is one of my peers
            AddPeer(aPlatformPtr);

            // Update the default command chain if necessary.
            if (defChainPtr)
            {
               defChainPtr->AddPeer(aPlatformPtr);
            }
         }
      }
   }
}

// ================================================================================================
//! Perform cleanup associated with a platform's being deleted from the simulation.
//! @param aPlatformPtr The platform (WsfPlatform*) that is about to be deleted.
void WsfCommandChain::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   std::vector<WsfPlatform*>::iterator iter;

   WsfCommandChain* defChainPtr = nullptr;
   if (GetNameId() != GetDefaultNameId())
   {
      defChainPtr = mPlatformPtr->GetComponent<WsfCommandChain>(GetDefaultNameId());
   }

   if (mCommanderPtr == aPlatformPtr)
   {
      // My commander is being deleted.
      mCommanderPtr = nullptr;

      // Assume SELF command since commander is deleted
      mCommanderPtr  = mPlatformPtr;
      mCommanderName = mPlatformPtr->GetNameId();

      // The script may set a new commander
      // Execute any script - commander deletion
      if (mPlatformPtr->HasScript(std::string("on_commander_deleted")))
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         // There is one argument; the deleted platform
         scriptArgs.push_back(UtScriptData(new UtScriptRef(const_cast<WsfPlatform*>(aPlatformPtr), mPlatformClassPtr)));

         mPlatformPtr->ExecuteScript(mPlatformPtr->GetSimTime(), "on_commander_deleted", scriptRetVal, scriptArgs);
      }
   }
   if ((iter = find(mPeers.begin(), mPeers.end(), aPlatformPtr)) != mPeers.end())
   {
      // Execute any script - peer deletion
      if (mPlatformPtr->HasScript(std::string("on_peer_deleted")))
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         // There is one argument; the deleted platform
         scriptArgs.push_back(UtScriptData(new UtScriptRef(const_cast<WsfPlatform*>(aPlatformPtr), mPlatformClassPtr)));

         mPlatformPtr->ExecuteScript(mPlatformPtr->GetSimTime(), "on_peer_deleted", scriptRetVal, scriptArgs);
      }

      // One of my peers is being deleted.
      mPeers.erase(iter);

      if (defChainPtr)
      {
         defChainPtr->RemovePeer(aPlatformPtr);
      }
   }
   if ((iter = find(mSubordinates.begin(), mSubordinates.end(), aPlatformPtr)) != mSubordinates.end())
   {
      // Execute any script - subordinate deletion
      if (mPlatformPtr->HasScript((std::string) "on_subordinate_deleted"))
      {
         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;

         // There is one argument; the deleted platform
         scriptArgs.push_back(UtScriptData(new UtScriptRef(const_cast<WsfPlatform*>(aPlatformPtr), mPlatformClassPtr)));

         mPlatformPtr->ExecuteScript(mPlatformPtr->GetSimTime(), "on_subordinate_deleted", scriptRetVal, scriptArgs);
      }

      // One of my direct subordinates is being deleted.
      mSubordinates.erase(iter);

      if (defChainPtr)
      {
         defChainPtr->RemoveSubordinate(aPlatformPtr);
      }
   }
}

// ================================================================================================
// private
std::string WsfCommandChain::PrintHeader() const
{
   return "Platform " + mPlatformPtr->GetName() + '.' + GetNameId();
}

// ================================================================================================
//! The script interface 'class'
class WSF_EXPORT WsfScriptCommandChainClass : public UtScriptClass
{
public:
   WsfScriptCommandChainClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Commander);
   UT_DECLARE_SCRIPT_METHOD(CommanderName);
   UT_DECLARE_SCRIPT_METHOD(Peers);
   UT_DECLARE_SCRIPT_METHOD(Subordinates);
   UT_DECLARE_SCRIPT_METHOD(PeerCount);
   UT_DECLARE_SCRIPT_METHOD(PeerEntry);
   UT_DECLARE_SCRIPT_METHOD(SubordinateCount);
   UT_DECLARE_SCRIPT_METHOD(SubordinateEntry);
   UT_DECLARE_SCRIPT_METHOD(Name);
};

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfCommandChain::CreateScriptClass(const std::string& aClassName,
                                                                  UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptCommandChainClass>(aClassName, aScriptTypesPtr);
}

WsfScriptCommandChainClass::WsfScriptCommandChainClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommandChain");

   AddMethod(ut::make_unique<Commander>());
   AddMethod(ut::make_unique<CommanderName>());
   AddMethod(ut::make_unique<Peers>());
   AddMethod(ut::make_unique<Subordinates>());
   AddMethod(ut::make_unique<PeerCount>());
   AddMethod(ut::make_unique<PeerEntry>());
   AddMethod(ut::make_unique<SubordinateCount>());
   AddMethod(ut::make_unique<SubordinateEntry>());
   AddMethod(ut::make_unique<Name>());
}

//! Return the platform that is the commander of this platform.
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, Commander, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr = aObjectPtr->GetCommander();
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

//! Return the name of platform that is declared to be the commander of this platform.
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, CommanderName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetCommanderName());
}

//! Return the list of platforms that are the peers of this platform.
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, Peers, 0, "WsfPlatformList", "")
{
   WsfCommandChain::PlatformList* platformListPtr = &(aObjectPtr->GetPeers());
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr));
}

//! Return the list of platforms that are the subordinates of this platform.
UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, Subordinates, 0, "WsfPlatformList", "")
{
   WsfCommandChain::PlatformList* platformListPtr = &(aObjectPtr->GetSubordinates());
   aReturnVal.SetPointer(new UtScriptRef(platformListPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, PeerCount, 0, "int", "")
{
   WsfCommandChain::PlatformList& platformList = aObjectPtr->GetPeers();
   aReturnVal.SetInt(static_cast<int>(platformList.size()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, PeerEntry, 1, "WsfPlatform", "int")
{
   // Argument 1: int aEntryIndex
   WsfCommandChain::PlatformList& platformList = aObjectPtr->GetPeers();
   int                            entryIndex   = aVarArgs[0].GetInt();
   WsfPlatform*                   platformPtr  = platformList[entryIndex];
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, SubordinateCount, 0, "int", "")
{
   WsfCommandChain::PlatformList& platformList = aObjectPtr->GetSubordinates();
   aReturnVal.SetInt(static_cast<int>(platformList.size()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, SubordinateEntry, 1, "WsfPlatform", "int")
{
   // Argument 1: int aEntryIndex
   WsfCommandChain::PlatformList& platformList = aObjectPtr->GetSubordinates();
   int                            entryIndex   = aVarArgs[0].GetInt();
   WsfPlatform*                   platformPtr  = platformList[entryIndex];
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptCommandChainClass, WsfCommandChain, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}
