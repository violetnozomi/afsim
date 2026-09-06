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

#include "WsfExchangeProcessor.hpp"

#include <cmath>
#include <float.h>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtVec3dX.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfExchangeObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

namespace
{
// The values below MUST match those in DisCapabilities.hpp.
const unsigned int cPAYLOAD_SUPPLY_BIT  = 0; // bit 0
const unsigned int cFUEL_SUPPLY_BIT     = 1; // bit 1
const unsigned int cRECOVERY_SUPPLY_BIT = 2; // bit 2
const unsigned int cREPAIR_SUPPLY_BIT   = 3; // bit 3
const unsigned int cADS_B_SUPPLY_BIT    = 4; // bit 4
} // namespace

// ================================================================================================
// virtual
const char* WsfExchangeProcessor::GetScriptClassName() const
{
   return "WsfExchangeProcessor";
}

// ============================================================================
// static
UtScriptClass* WsfExchangeProcessor::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return new WsfScriptExchangeProcessorClass(aClassName, aScriptTypesPtr);
}

// ============================================================================
WsfExchangeProcessor::WsfExchangeProcessor(WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario)
   , mTransactors()
   , mCapabilityPairs()
   , mKnownSuppliers()
   , mContainers()
   , mCallbacks()
   , mIgnoreProxChecks(false)
   , mAllTransAreInstant(false)
   , mOnTransactionCompleteScriptPtr(nullptr)
   , mOnRequestFailedScriptPtr(nullptr)
   , mTransactorClassPtr(nullptr) // = mContextPtr->GetClass("Transactor");
{
   SetUpdateInterval(10.0); // default
   mTransactorClassPtr = mContextPtr->GetClass("Transactor");

   WsfExchange::InitializeStatics();
}

// ============================================================================
WsfExchangeProcessor::WsfExchangeProcessor(const WsfExchangeProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mTransactors(aSrc.mTransactors)
   , mCapabilityPairs(aSrc.mCapabilityPairs)
   , mKnownSuppliers(aSrc.mKnownSuppliers)
   , mContainers(aSrc.mContainers)
   , mCallbacks(aSrc.mCallbacks)
   , mIgnoreProxChecks(aSrc.mIgnoreProxChecks)
   , mAllTransAreInstant(aSrc.mAllTransAreInstant)
   , mOnTransactionCompleteScriptPtr(nullptr)
   , mOnRequestFailedScriptPtr(nullptr)
   , mTransactorClassPtr(nullptr) // = mContextPtr->GetClass("Transactor");
{
   mTransactorClassPtr = mContextPtr->GetClass("Transactor");
}

// ============================================================================
// virtual
WsfExchangeProcessor::~WsfExchangeProcessor() {}

// ============================================================================
// virtual
WsfProcessor* WsfExchangeProcessor::Clone() const
{
   return new WsfExchangeProcessor(*this);
}

// ============================================================================
WsfExchange::Container* WsfExchangeProcessor::FindContainer(WsfStringId aContainerNameId)
{
   for (size_t i = 0; i != mContainers.size(); ++i)
   {
      if (mContainers[i].GetNameId() == aContainerNameId)
      {
         return &mContainers[i];
      }
   }
   return nullptr;
}

// ============================================================================
WsfExchange::Transactor* WsfExchangeProcessor::FindTransactor(WsfStringId aTransactorNameId)
{
   for (size_t i = 0; i != mTransactors.size(); ++i)
   {
      if (mTransactors[i].GetNameId() == aTransactorNameId)
      {
         return &mTransactors[i];
      }
   }
   return nullptr;
}

// ============================================================================
// virtual
bool WsfExchangeProcessor::Initialize(double aSimTime)
{
   // Initialize all of our sub-components, and assure all is ready for use.
   bool success = WsfScriptProcessor::Initialize(aSimTime);

   if (success)
   {
      size_t i;
      for (i = 0; i != mContainers.size(); ++i)
      {
         if (!mContainers[i].Initialize())
         {
            auto out = ut::log::error() << "Platform's container failed initialization.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Container: " << mContainers[i].GetName();
            success = false;
         }
      }

      for (i = 0; i != mTransactors.size(); ++i)
      {
         if (!mTransactors[i].Initialize(aSimTime, this))
         {
            auto out = ut::log::error() << "Platform's transactor failed initialization.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Transactor: " << mTransactors[i].GetName();
            success = false;
         }
      }
   }

   if (success)
   {
      // Capture the pointer to the 'on_message_create' script and ensure it has the proper signature.
      mOnTransactionCompleteScriptPtr = mContextPtr->FindScript("on_transaction_complete");
      if (mOnTransactionCompleteScriptPtr != nullptr)
      {
         success &= mContextPtr->ValidateScript(mOnTransactionCompleteScriptPtr, "void", "Transactor");
         if (!success)
         {
            auto out = ut::log::error() << "Could not validate the 'on_transaction_complete' script.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Exchange Processor: " << GetName();
         }
      }
   }

   if (success)
   {
      // Capture the pointer to the 'on_request_failed' script and ensure it has the proper signature.
      mOnRequestFailedScriptPtr = mContextPtr->FindScript("on_request_failed");
      if (mOnRequestFailedScriptPtr != nullptr)
      {
         success &= mContextPtr->ValidateScript(mOnRequestFailedScriptPtr, "void", "Transactor");
         if (!success)
         {
            auto out = ut::log::error() << "Could not validate the 'on_request_failed' script.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Exchange Processor: " << GetName();
         }
      }
   }

   if (success)
   {
      mCallbacks.Add(WsfObserver::PlatformCapabilityChanged(GetSimulation())
                        .Connect(&WsfExchangeProcessor::PlatformCapabilityChanged, this));

      mCallbacks.Add(WsfObserver::ExchangeQueried(GetSimulation()).Connect(&WsfExchangeProcessor::ExchangeQueried, this));

      // mCallbacks.Add(WsfObserver::ExchangeNegotiated(
      //    GetSimulation()).Connect(&WsfExchangeProcessor::ExchangeNegotiated, this));

      mCallbacks.Add(
         WsfObserver::ExchangeRequestFailed(GetSimulation()).Connect(&WsfExchangeProcessor::ExchangeRequestFailed, this));

      mCallbacks.Add(
         WsfObserver::ExchangeCompleted(GetSimulation()).Connect(&WsfExchangeProcessor::ExchangeCompleted, this));
   }

   return success;
}

// ============================================================================
void WsfExchangeProcessor::ExchangeQueried(double aSimTime, const WsfExchange::Query& aQuery)
{
   bool queryWasMatched = false;
   for (size_t iTrans = 0; (!queryWasMatched) && (iTrans < mTransactors.size()); iTrans++)
   {
      if (mTransactors[iTrans].ExchangeQueried(aSimTime, aQuery))
      {
         queryWasMatched = true;
      }
   }
}

// ============================================================================
/*void WsfExchangeProcessor::ExchangeNegotiated(double                    aSimTime,
                                              const WsfExchange::Query& aQuery)
{
   bool queryWasMatched = false;
   for (size_t iTrans = 0; (! queryWasMatched) && (iTrans < mTransactors.size()); iTrans++)
   {
      if (mTransactors[iTrans].ExchangeNegotiated(aSimTime, aQuery))
      {
         queryWasMatched = true;
      }
   }
}*/

// ============================================================================
void WsfExchangeProcessor::ExchangeRequestFailed(double                         aSimTime,
                                                 const WsfExchange::Transactor& aTransactor,
                                                 const WsfExchange::Query&      aQuery)
{
   size_t myIndex = GetPlatform()->GetIndex();

   if (aQuery.RequestingIndex() == myIndex || aQuery.RespondingIndex() == myIndex)
   {
      // This transaction did involve me as a participant:
      for (size_t iTrans = 0; iTrans < mTransactors.size(); iTrans++)
      {
         unsigned int rid1 = mTransactors[iTrans].GetQuery().RequestId();
         // unsigned int rid2 = aTransactor.GetQuery().RequestId();
         unsigned int rid3 = aQuery.RequestId();

         if (rid1 == rid3)
         {
            // I found a match to the transactor used in this transaction:
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Transaction failed.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Exchange Processor: " << GetName();
               out.AddNote() << "Transactor: " << aTransactor.GetName();
               out.AddNote() << "Item: " << aQuery.ItemId();
               out.AddNote() << "Negotiated Quantity: " << aQuery.NegotiatedQuantity();
            }
            if (mOnRequestFailedScriptPtr != nullptr)
            {
               UtScriptData     scriptRetVal;
               UtScriptDataList scriptArgs;
               scriptArgs.push_back(UtScriptData(new UtScriptRef(&mTransactors[iTrans], mTransactorClassPtr)));
               mContextPtr->ExecuteScript(aSimTime, mOnRequestFailedScriptPtr, scriptRetVal, scriptArgs);
            }
            break;
         }
      }
   }
}

// ============================================================================
void WsfExchangeProcessor::ExchangeCompleted(double                         aSimTime,
                                             const WsfExchange::Transactor& aTransactor,
                                             const WsfExchange::Query&      aQuery)
{
   size_t myIndex = GetPlatform()->GetIndex();

   if ((aQuery.RequestingIndex() == myIndex) || (aQuery.RespondingIndex() == myIndex))
   {
      // This transaction did involve me as a participant.  Loop through all my
      // transactors to determine which one a user-defined script should be called on.

      for (size_t iTrans = 0; iTrans < mTransactors.size(); iTrans++)
      {
         unsigned int rid1 = mTransactors[iTrans].GetQuery().RequestId();
         // unsigned int rid2 = aTransactor.GetQuery().RequestId();
         unsigned int rid3 = aQuery.RequestId();

         if (rid1 == rid3)
         {
            // I found a match to the transactor used in this transaction:
            if (DebugEnabled())
            {
               auto out = ut::log::debug()
                          << "Transaction was " << (mTransactors[iTrans].IsOfferor() ? "received" : "supplied") << ".";
               out << " Negotiated Quantity of Item.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Item: " << aQuery.ItemId();
               out.AddNote() << "Negotiated Quantity: " << aQuery.NegotiatedQuantity();
               out.AddNote() << "If defined, will call a user-defined 'on_transaction_complete' script.";
            }
            if (mOnTransactionCompleteScriptPtr != nullptr)
            {
               UtScriptData     scriptRetVal;
               UtScriptDataList scriptArgs;
               scriptArgs.push_back(UtScriptData(new UtScriptRef(&mTransactors[iTrans], mTransactorClassPtr)));
               mContextPtr->ExecuteScript(aSimTime, mOnTransactionCompleteScriptPtr, scriptRetVal, scriptArgs);
            }

            break;
         }
      }
   }
}

// ============================================================================
void WsfExchangeProcessor::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   RemoveSupplier(aPlatformPtr->GetIndex());

   // If this platform is me, then disable my transactors.
   if (aPlatformPtr == GetPlatform())
   {
      for (size_t i = 0; i != mTransactors.size(); ++i)
      {
         mTransactors[i].Disable();
      }
   }
}

// ============================================================================
void WsfExchangeProcessor::RemoveSupplier(size_t aPlatformIndex)
{
   for (size_t i = 0; i != mKnownSuppliers.size(); ++i)
   {
      // Rather than actually remove the array item, just "nullify"  it.
      if (mKnownSuppliers[i].mPlatformIndex == aPlatformIndex)
      {
         mKnownSuppliers[i].Nullify();
         break;
      }
   }
}

// ============================================================================
void WsfExchangeProcessor::SetSupplierAbility(size_t aPlatformIndex, WsfExchange::CapabilityType aCapabilityType, bool aIsCapable)
{
   size_t i;
   for (i = 0; i != mKnownSuppliers.size(); ++i)
   {
      if (mKnownSuppliers[i].mPlatformIndex == aPlatformIndex)
      {
         bool changed = mKnownSuppliers[i].SetCapability(aCapabilityType, aIsCapable);
         if (changed && DebugEnabled())
         {
            WsfPlatform* platPtr = GetSimulation()->GetPlatformByIndex(aPlatformIndex);
            std::string  name    = "unknown";
            if (platPtr != nullptr)
            {
               name = platPtr->GetName();
            }

            auto out = ut::log::debug() << "Set supplier capability to " << std::boolalpha << aIsCapable << ".";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Exchange Platform: " << GetName();
            out.AddNote() << "Supplier: " << name;
            out.AddNote() << "Capability: " << CapabilityToNameId(aCapabilityType);
         }
         break;
      }
   }
}

// ============================================================================
WsfExchange::CapabilityType WsfExchangeProcessor::GetCapabilityOf(WsfStringId aCommodityTypeId)
{
   for (size_t i = 0; i != mCapabilityPairs.size(); ++i)
   {
      if (mCapabilityPairs[i].mCommodityTypeId == aCommodityTypeId)
      {
         return mCapabilityPairs[i].mCapability;
      }
   }
   return WsfExchange::cNO_CAPABILITY;
}

// ============================================================================
// protected
bool WsfExchangeProcessor::AddCapability(WsfStringId aCommodityTypeId, WsfExchange::CapabilityType aCapabilityType)
{
   // Add a new capability provider into our "available" list.
   for (size_t i = 0; i != mCapabilityPairs.size(); ++i)
   {
      if (mCapabilityPairs[i].mCommodityTypeId == aCommodityTypeId)
      {
         return false;
      }
   }
   mCapabilityPairs.push_back(CommodityCapability(aCommodityTypeId, aCapabilityType));

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform's Exchange Processor added commodity capability.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Exchange Processor: " << GetName();
      out.AddNote() << "Commodity: " << aCommodityTypeId;
      out.AddNote() << "Capability: " << CapabilityToNameId(aCapabilityType);
   }
   return true;
}

// ============================================================================

WsfPlatform* WsfExchangeProcessor::ClosestPossibleProvider(WsfStringId aCommodityOrServiceId, double aProximityLimit)
{
   // Traverse the list of providers we know about, and return the closest
   // provider of such a capability.

   if (mKnownSuppliers.empty())
   {
      return nullptr;
   }

   // string cName = aCommodityOrServiceId.GetString();
   WsfExchange::CapabilityType capability = GetCapabilityOf(aCommodityOrServiceId);
   if (capability == WsfExchange::cNO_CAPABILITY)
   {
      return nullptr;
   }

   size_t       platIndexToRemove     = 0;
   WsfPlatform* closestPlatformPtr    = nullptr;
   double       closestPlatformOffset = std::numeric_limits<double>::max();

   for (size_t i = 0; i != mKnownSuppliers.size(); ++i)
   {
      if (mKnownSuppliers[i].IsCapableOf(capability))
      {
         WsfPlatform* hisPlatPtr = GetSimulation()->GetPlatformByIndex(mKnownSuppliers[i].mPlatformIndex);
         if (hisPlatPtr == nullptr)
         {
            // Clean up our list, as this guy appears to have died...
            platIndexToRemove = mKnownSuppliers[i].mPlatformIndex;
         }
         else
         {
            UtVec3dX hisLocWCS;
            hisPlatPtr->GetLocationWCS(hisLocWCS.GetData());
            UtVec3dX myLocWCS;
            GetPlatform()->GetLocationWCS(myLocWCS.GetData());
            double proximity = (hisLocWCS - myLocWCS).Magnitude();
            if ((proximity <= closestPlatformOffset) && ((proximity <= aProximityLimit) || (aProximityLimit == 0.0)))
            {
               closestPlatformPtr    = hisPlatPtr;
               closestPlatformOffset = proximity;
            }
         }
      }
   }

   if (platIndexToRemove != 0)
   {
      RemoveSupplier(platIndexToRemove);
   }

   return closestPlatformPtr;
}

// ============================================================================
// virtual
bool WsfExchangeProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   WsfExchange::Transactor* transPtr = nullptr;
   WsfExchange::Container*  contPtr  = nullptr;

   if (command == "ignore_all_proximity_checks")
   {
      aInput.ReadValue(mIgnoreProxChecks);
   }
   else if (command == "force_transactions_instantaneous")
   {
      aInput.ReadValue(mAllTransAreInstant);
   }
   else if (WsfExchange::Transactor::LoadInstance(aInput, transPtr))
   {
      if (!AddTransactor(*transPtr))
      {
         std::string name = transPtr->GetName();
         throw UtInput::BadValue(aInput, "Transactor " + name + " could not be added.  Name may be a duplicate.");
      }
      delete transPtr;
      transPtr = nullptr;
   }
   else if (WsfExchange::Container::LoadInstance(aInput, contPtr))
   {
      if (!AddContainer(*contPtr))
      {
         std::string name = contPtr->GetName();
         throw UtInput::BadValue(aInput, "Container " + name + " could not be added.  Name may be a duplicate.");
      }
      delete contPtr;
      transPtr = nullptr;
   }
   else if ((command == "commodity_and_capability_pairing") || (command == "service_and_capability_pairing"))
   {
      std::string word = "commodity";
      if (command == "service_and_capability_pairing")
      {
         word = "service";
      }
      std::string commodityTypeName;
      aInput.ReadValue(commodityTypeName);
      WsfStringId commodityNameId = commodityTypeName;

      std::string capabilityMatchName;
      aInput.ReadValue(capabilityMatchName);
      WsfExchange::CapabilityType capabilityType;

      if ((capabilityMatchName == "SUPPLY_FUEL") || (capabilityMatchName == "supply_fuel"))
      {
         capabilityType = WsfExchange::cSUPPLY_FUEL;
      }
      else if ((capabilityMatchName == "SUPPLY_PAYLOAD") || (capabilityMatchName == "supply_payload"))
      {
         capabilityType = WsfExchange::cSUPPLY_PAYLOAD;
      }
      else if ((capabilityMatchName == "VEHICLE_REPAIR") || (capabilityMatchName == "vehicle_repair"))
      {
         capabilityType = WsfExchange::cVEHICLE_REPAIR;
      }
      else if ((capabilityMatchName == "VEHICLE_RECOVERY") || (capabilityMatchName == "vehicle_recovery"))
      {
         capabilityType = WsfExchange::cVEHICLE_RECOVERY;
      }
      else if ((capabilityMatchName == "SUPPLY_ADS_B") || (capabilityMatchName == "supply_ads_b"))
      {
         capabilityType = WsfExchange::cSUPPLY_ADS_B;
      }
      else
      {
         throw UtInput::BadValue(
            aInput,
            "Unknown " + word + " type '" + capabilityMatchName +
               "', use SUPPLY_FUEL, SUPPLY_PAYLOAD, VEHICLE_REPAIR, VEHICLE_RECOVERY, or SUPPLY_ADS_B.");
      }

      if (!AddCapability(commodityNameId, capabilityType))
      {
         throw UtInput::BadValue(aInput,
                                 "Could not add " + word + " " + commodityTypeName + " as a capability type " +
                                    capabilityMatchName);
      }
   }
   else if (command == "edit")
   {
      std::string componentType;
      aInput.ReadValue(componentType);
      bool editTransactor = false;
      if (componentType == "transactor")
      {
         editTransactor = true;
      }
      else if (componentType == "container")
      {
         // Do nothing.
      }
      else
      {
         throw UtInput::BadValue(aInput, "Valid 'edit' types are transactor and container only.");
      }
      std::string componentName;
      aInput.ReadValue(componentName);
      if (editTransactor)
      {
         WsfExchange::Transactor* tPtr = FindTransactor(WsfStringId(componentName));
         std::string              msg  = "Transactor named " + componentName + " not found.";
         if (tPtr == nullptr)
         {
            throw UtInput::BadValue(aInput, msg);
         }
         UtInputBlock block(aInput, "end_transactor");
         while (block.ReadCommand())
         {
            if (!tPtr->ProcessInput(block.GetInput()))
            {
               throw UtInput::BadValue(block.GetInput());
            }
         }
      }
      else
      {
         WsfExchange::Container* cPtr = FindContainer(WsfStringId(componentName));
         std::string             msg  = "Container named " + componentName + " not found.";
         if (cPtr == nullptr)
         {
            throw UtInput::BadValue(aInput, msg);
         }
         UtInputBlock block(aInput, "end_container");
         while (block.ReadCommand())
         {
            if (!cPtr->ProcessInput(block.GetInput()))
            {
               throw UtInput::BadValue(block.GetInput());
            }
         }
      }
   }
   else
   {
      myCommand = WsfScriptProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// ============================================================================
bool WsfExchangeProcessor::GetTransactorPtr(WsfStringId               aItemNameId,
                                            WsfStringId               aContainerId,
                                            bool                      aIsOfferor,
                                            WsfExchange::Transactor*& aTransPtr)
{
   for (size_t i = 0; i != mTransactors.size(); ++i)
   {
      if ((mTransactors[i].GetQuery().ItemId() == aItemNameId) && (mTransactors[i].ContainerNameId() == aContainerId) &&
          (mTransactors[i].IsOfferor() == aIsOfferor))
      {
         aTransPtr = &mTransactors[i];
         return true;
      }
   }
   return false;
}

// ============================================================================
bool WsfExchangeProcessor::AddTransactor(const WsfExchange::Transactor& aTransactor)
{
   // First, have to see if there are any conflicts before adding.
   for (size_t i = 0; i != mTransactors.size(); ++i)
   {
      if (mTransactors[i].IsInConflictWith(aTransactor))
      {
         return false;
      }
   }

   // Transactor of this name already exists, so fail this addition.
   mTransactors.push_back(aTransactor);
   return true;
}

// ============================================================================
bool WsfExchangeProcessor::AddContainer(const WsfExchange::Container& aContainer)
{
   // First, have to see if there are any conflicts before adding.
   WsfExchange::Container* contPtr = FindContainer(aContainer.GetNameId());
   if (contPtr != nullptr)
   {
      // Container of this name already exists, so fail this addition.
      return false;
   }
   mContainers.push_back(aContainer);
   return true;
}

// ============================================================================
void WsfExchangeProcessor::PlatformCapabilityChanged(double       aSimTime,
                                                     WsfPlatform* aPlatformPtr,
                                                     WsfStringId  aCapabilityId,
                                                     bool         aIsCapable)
{
   // Ignore our own state changes:
   if (aPlatformPtr == GetPlatform())
   {
      return;
   }

   bool considered = false;

   WsfExchange::CapabilityType capability = WsfExchange::NameIdToCapability(aCapabilityId);

   // Go through our known other platforms capabilities, and update accordingly:
   for (size_t i = 0; i != mKnownSuppliers.size(); ++i)
   {
      if (mKnownSuppliers[i].mPlatformIndex == aPlatformPtr->GetIndex())
      {
         considered = true;
         mKnownSuppliers[i].SetCapability(capability, aIsCapable);
         break;
      }
   }

   if (!considered)
   {
      // Did not find this platform, add it:
      mKnownSuppliers.push_back(Supplier(aPlatformPtr->GetIndex()));

      // Now set the capability flag as given to us.
      mKnownSuppliers.back().SetCapability(capability, aIsCapable);
   }
}

// =========================================================================
std::vector<size_t> WsfExchangeProcessor::PayloadProviders()
{
   std::vector<size_t> result;
   for (size_t i = 0; i != mKnownSuppliers.size(); ++i)
   {
      if (mKnownSuppliers[i].IsCapableOf(WsfExchange::cSUPPLY_PAYLOAD))
      {
         result.push_back(mKnownSuppliers[i].mPlatformIndex);
      }
   }
   return result;
}

// =========================================================================
std::vector<size_t> WsfExchangeProcessor::FuelProviders()
{
   std::vector<size_t> result;
   for (size_t i = 0; i != mKnownSuppliers.size(); ++i)
   {
      if (mKnownSuppliers[i].IsCapableOf(WsfExchange::cSUPPLY_FUEL))
      {
         result.push_back(mKnownSuppliers[i].mPlatformIndex);
      }
   }
   return result;
}

// =========================================================================
std::vector<size_t> WsfExchangeProcessor::VehicleRepairers()
{
   std::vector<size_t> result;
   for (size_t i = 0; i != mKnownSuppliers.size(); ++i)
   {
      if (mKnownSuppliers[i].IsCapableOf(WsfExchange::cVEHICLE_REPAIR))
      {
         result.push_back(mKnownSuppliers[i].mPlatformIndex);
      }
   }
   return result;
}

// =========================================================================
std::vector<size_t> WsfExchangeProcessor::VehicleRecoverers()
{
   std::vector<size_t> result;
   for (size_t i = 0; i != mKnownSuppliers.size(); ++i)
   {
      if (mKnownSuppliers[i].IsCapableOf(WsfExchange::cVEHICLE_RECOVERY))
      {
         result.push_back(mKnownSuppliers[i].mPlatformIndex);
      }
   }
   return result;
}

// =========================================================================

// ============================================================================
// ======= START SUB_CLASS SUPPLIER ======= START SUB-CLASS SUPPLIER ==========
// ============================================================================

WsfExchangeProcessor::Supplier::Supplier(size_t aPlatformIndex)
   : mPlatformIndex(aPlatformIndex)
{
   Nullify();
}

// ===========================================================================
void WsfExchangeProcessor::Supplier::Nullify()
{
   // Here the cNO_CAPABILITY value is the upper bound value for the enum.
   for (size_t i = 0; i != WsfExchange::cNO_CAPABILITY; ++i)
   {
      mAbilities[i] = false;
   }
}

// ===========================================================================
bool WsfExchangeProcessor::Supplier::SetCapability(WsfExchange::CapabilityType aAbility, bool aIsCapable)
{
   // size_t i = WsfExchange::BitNumberFromEnum(aAbility);
   bool changed         = mAbilities[aAbility] != aIsCapable;
   mAbilities[aAbility] = aIsCapable;
   return changed;
}

// ===========================================================================
bool WsfExchangeProcessor::Supplier::IsCapableOf(WsfExchange::CapabilityType aAbility) const
{
   return mAbilities[aAbility];
}

// ============================================================================
// ============= START SCRIPT CLASS ======= START SCRIPT CLASS ================
// ============================================================================

WsfScriptExchangeProcessorClass::WsfScriptExchangeProcessorClass(const std::string& aClassName,
                                                                 UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfExchangeProcessor");

   AddMethod(ut::make_unique<FindContainer>());
   AddMethod(ut::make_unique<FindTransactor>());

   AddMethod(ut::make_unique<PayloadProviders>());
   AddMethod(ut::make_unique<FuelProviders>());
   AddMethod(ut::make_unique<VehicleRepairers>());
   AddMethod(ut::make_unique<VehicleRecoverers>());
   // AddMethod(ut::make_unique<ADS_B_Suppliers>());

   AddMethod(ut::make_unique<ClosestPossibleProvider>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExchangeProcessorClass, WsfExchangeProcessor, FindTransactor, 1, "Transactor", "string")
{
   WsfExchange::Transactor* tPtr = aObjectPtr->FindTransactor(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(tPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExchangeProcessorClass, WsfExchangeProcessor, FindContainer, 1, "Container", "string")
{
   WsfExchange::Container* cPtr = aObjectPtr->FindContainer(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(cPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExchangeProcessorClass, WsfExchangeProcessor, PayloadProviders, 0, "Array<int>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();
   std::vector<size_t>        result       = aObjectPtr->PayloadProviders();
   for (size_t i = 0; i != result.size(); ++i)
   {
      int aResult = static_cast<int>(result[i]);
      resultVecPtr->push_back(UtScriptData(aResult));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExchangeProcessorClass, WsfExchangeProcessor, FuelProviders, 0, "Array<int>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();
   std::vector<size_t>        result       = aObjectPtr->FuelProviders();
   for (size_t i = 0; i != result.size(); ++i)
   {
      int aResult = static_cast<int>(result[i]);
      resultVecPtr->push_back(UtScriptData(aResult));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExchangeProcessorClass, WsfExchangeProcessor, VehicleRepairers, 0, "Array<int>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();
   std::vector<size_t>        result       = aObjectPtr->VehicleRepairers();
   for (size_t i = 0; i != result.size(); ++i)
   {
      int aResult = static_cast<int>(result[i]);
      resultVecPtr->push_back(UtScriptData(aResult));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExchangeProcessorClass, WsfExchangeProcessor, VehicleRecoverers, 0, "Array<int>", "")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();
   std::vector<size_t>        result       = aObjectPtr->VehicleRecoverers();
   for (size_t i = 0; i != result.size(); ++i)
   {
      int aResult = static_cast<int>(result[i]);
      resultVecPtr->push_back(UtScriptData(aResult));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptExchangeProcessorClass, WsfExchangeProcessor, ClosestPossibleProvider, 1, "WsfPlatform", "string")
{
   WsfStringId  stuffId(aVarArgs[0].GetString());
   WsfPlatform* providerPtr = aObjectPtr->ClosestPossibleProvider(stuffId);
   aReturnVal.SetPointer(new UtScriptRef(providerPtr, aReturnClassPtr));
}
