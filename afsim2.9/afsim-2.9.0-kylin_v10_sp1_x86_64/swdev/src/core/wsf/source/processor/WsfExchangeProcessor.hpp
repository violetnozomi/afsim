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

#ifndef WSFEXCHANGEPROCESSOR_HPP
#define WSFEXCHANGEPROCESSOR_HPP

#include <float.h>
#include <vector>

#include "UtCallbackHolder.hpp"
class UtInput;
#include "wsf_export.h"

#include "WsfEvent.hpp"
#include "WsfExchange.hpp"
#include "WsfMessage.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptProcessorClass.hpp"

//! The WsfExchangeProcessor controls the exchange of goods or services from one
//! simulated platform to another, potentially externally via DIS if necessary.
//! It allows for the owning platform to simultaneously request or supply more
//! than one commodity or service, through the including of multiple Transactors
//! onto the processor.  For each Transactor that is an Offeror of commodities or
//! services, a capability bit is exported, to permit interested platforms to know
//! to whom they might address a request, and where to go for rendezvous with same.
//! (This is not fully robust, as while a platform claims to supply fuel, it may
//!  supply a different kind of fuel than is desired, so will not respond when asked.)
//! So an Update() of the processor is going to populate the platform's outbound
//! capability bits, publishing what it may provide, and then will also update an
//! internal list of what platforms might potentially be able to supply the
//! commodities or services desired by the platform.  A transfer always begins
//! with a Request, and the addressee will (only if able) respond with an
//! Offer.  See Wiki Docs for details (WsfExchange, WsfWExchangeProcessor).

class WSF_EXPORT WsfExchangeProcessor : public WsfScriptProcessor
{
public:
   WsfExchangeProcessor(WsfScenario& aScenario);
   WsfExchangeProcessor(const WsfExchangeProcessor& aSrc);
   WsfExchangeProcessor& operator=(const WsfExchangeProcessor&) = delete;
   ~WsfExchangeProcessor() override;

   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   bool IgnoreAllProximityChecks() const { return mIgnoreProxChecks; }
   bool ForceTransactionsInstantaneous() const { return mAllTransAreInstant; }

   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   // ====================== Container Accessors =========================
   WsfExchange::Container* FindContainer(WsfStringId aContainerNameId);
   bool                    AddContainer(const WsfExchange::Container& aContainer);

   // ====================== Transactor Accessors =========================
   bool                     AddTransactor(const WsfExchange::Transactor& aTransactor);
   WsfExchange::Transactor* FindTransactor(WsfStringId aTransactorNameId);
   bool                     GetTransactorPtr(WsfStringId aItemNameId, WsfStringId aContainerId, bool aIsOfferor, WsfExchange::Transactor*& aTransPtr);

   // ====================== Capability Accessors =========================
   WsfExchange::CapabilityType GetCapabilityOf(WsfStringId aCommodityNameId);
   bool                        AddCapability(WsfStringId aCommodityTypeId);

   // ================ Callback Signatures =============================
   void PlatformCapabilityChanged(double aSimTime, WsfPlatform* aPlatformPtr, WsfStringId aCapabilityId, bool aIsCapable);
   void ExchangeQueried(double aSimTime, const WsfExchange::Query& aQuery);
   void ExchangeNegotiated(double aSimTime, const WsfExchange::Query& aQuery);
   void ExchangeRequestFailed(double aSimTime, const WsfExchange::Transactor& aTransactor, const WsfExchange::Query& aQuery);
   void ExchangeCompleted(double aSimTime, const WsfExchange::Transactor& aTransactor, const WsfExchange::Query& aQuery);

   // ================ Script-Related  =============================
   static UtScriptClass* CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   const char*           GetScriptClassName() const override;

   std::vector<size_t> PayloadProviders();
   std::vector<size_t> FuelProviders();
   std::vector<size_t> VehicleRecoverers();
   std::vector<size_t> VehicleRepairers();
   // std::vector<size_t> ADS_B_Providers();

   WsfPlatform* ClosestPossibleProvider(WsfStringId aCommodityOrServiceId,
                                        double      aProximityLimit = std::numeric_limits<double>::max());
   // =========================================================================

protected:
   struct CommodityCapability
   {
      CommodityCapability(WsfStringId aCommodityTypeId, WsfExchange::CapabilityType aCapability)
         : mCommodityTypeId(aCommodityTypeId)
         , mCapability(aCapability)
      {
      }

      WsfStringId                 mCommodityTypeId;
      WsfExchange::CapabilityType mCapability;
   };

   struct Supplier
   {
      Supplier(size_t aPlatformIndex);
      void Nullify();
      bool SetCapability(WsfExchange::CapabilityType aAbility, bool aIsCapable);
      bool IsCapableOf(WsfExchange::CapabilityType aAbility) const;

      size_t mPlatformIndex;
      bool   mAbilities[WsfExchange::cNO_CAPABILITY];
   };

   bool AddCapability(WsfStringId aCommodityTypeId, WsfExchange::CapabilityType aCapabilityType);

   void RemoveSupplier(size_t aPlatformIndex);

   void SetSupplierAbility(size_t aPlatformIndex, WsfExchange::CapabilityType aCapabilityType, bool aIsCapable);

private:
   std::vector<WsfExchange::Transactor> mTransactors; // Transactors I have been configured with in "ProcessInput()".
   std::vector<CommodityCapability>     mCapabilityPairs; // Collection of the capabilities I (this) provides.
   std::vector<Supplier>                mKnownSuppliers;  // Collection of affirmative "offerors/providers/suppliers".
   std::vector<WsfExchange::Container> mContainers; // Collection of Containers to hold pools of commodities or services.
   UtCallbackHolder                    mCallbacks;
   bool                                mIgnoreProxChecks;
   bool                                mAllTransAreInstant;
   UtScript*      mOnTransactionCompleteScriptPtr; //! Pointer to the 'on_transaction_complete' script.
   UtScript*      mOnRequestFailedScriptPtr;       //! Pointer to the 'on_request_failed' script.
   UtScriptClass* mTransactorClassPtr;             // = mContextPtr->GetClass("Transactor");
};

//! Define script methods for WsfProcessor.
class WSF_EXPORT WsfScriptExchangeProcessorClass : public WsfScriptProcessorClass
{
public:
   WsfScriptExchangeProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(FindTransactor);
   UT_DECLARE_SCRIPT_METHOD(FindContainer);

   UT_DECLARE_SCRIPT_METHOD(PayloadProviders);
   UT_DECLARE_SCRIPT_METHOD(FuelProviders);
   UT_DECLARE_SCRIPT_METHOD(VehicleRepairers);
   UT_DECLARE_SCRIPT_METHOD(VehicleRecoverers);
   // UT_DECLARE_SCRIPT_METHOD(ADS_B_Suppliers);

   UT_DECLARE_SCRIPT_METHOD(ClosestPossibleProvider);
};

#endif
