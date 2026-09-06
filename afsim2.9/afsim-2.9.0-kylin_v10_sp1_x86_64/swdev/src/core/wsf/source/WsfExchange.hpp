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

#ifndef WSFEXCHANGE_HPP
#define WSFEXCHANGE_HPP

#include <string>

#include "UtCallbackHolder.hpp"
#include "UtLog.hpp"
class UtInput;
#include "UtScriptAccessible.hpp"
#include "WsfEvent.hpp"
class WsfExchangeProcessor;
#include "wsf_export.h"

#include "WsfNamed.hpp"
class WsfPlatform;

#include "WsfStringId.hpp"

//! A namespace for platform-to-platform suppling or receiving of a commodity
//! or service.  Four contained classes:  Tender, Container, Query, and Transactor.
//! Requires a WsfExchangeProcessor to stimulate the callback events for
//! platform-to-platform Exchanges.  Terminology recommendation:  Avoid using
//! the term 'receiver', as in this context it may be ambiguous as to whether
//! it is the receiver of a _commodity_, or the receiver of an _event_ notif-
//! ication.  Recommend using "Originator" for the sender of a message or
//! event, "Requester" for player desiring commodities or services, and
//! "Responder" for the provider of the commodity or service.

namespace WsfExchange
{
/* TODO - VS2015 The following only works in VS2015 or gcc 5.x due to dll
                 deadlock in vs2013 and older for mutexes un dictionary call.
// Enumerated Transactor States:
static const WsfStringId sSTATE_READY("STATE_READY");
static const WsfStringId sSTATE_REQUESTING("STATE_REQUESTING");
static const WsfStringId sSTATE_ACCEPTING("STATE_ACCEPTING");
static const WsfStringId sSTATE_OFFERING("STATE_OFFERING");
static const WsfStringId sSTATE_SUPPLYING("STATE_SUPPLYING");

// Enumerated Transactor Events:
static const WsfStringId sEVENT_REQUESTED("EVENT_REQUESTED");
static const WsfStringId sEVENT_OFFERED("EVENT_OFFERED");
static const WsfStringId sEVENT_ACCEPTED("EVENT_ACCEPTED");
static const WsfStringId sEVENT_SUPPLIED("EVENT_SUPPLIED");
static const WsfStringId sEVENT_RECEIVED("EVENT_RECEIVED");
static const WsfStringId sEVENT_CANCELLED("EVENT_CANCELLED");
static const WsfStringId sEVENT_OFFEROR_COMPLETED("EVENT_OFFEROR_COMPLETED");
static const WsfStringId sEVENT_REQUESTOR_RESPONDED("EVENT_REQUESTOR_RESPONDED");
*/

void InitializeStatics();

// Enumerated Transactor States:
static WsfStringId sSTATE_READY;
static WsfStringId sSTATE_REQUESTING;
static WsfStringId sSTATE_ACCEPTING;
static WsfStringId sSTATE_OFFERING;
static WsfStringId sSTATE_SUPPLYING;

// Enumerated Transactor Events:
static WsfStringId sEVENT_REQUESTED;
static WsfStringId sEVENT_OFFERED;
static WsfStringId sEVENT_ACCEPTED;
static WsfStringId sEVENT_SUPPLIED;
static WsfStringId sEVENT_RECEIVED;
static WsfStringId sEVENT_CANCELLED;
static WsfStringId sEVENT_OFFEROR_COMPLETED;
static WsfStringId sEVENT_REQUESTOR_RESPONDED;

// ========================== CLASS TENDER =====================================
//! Tender, or offer of goods or services.  Represents the amount and rate of an
//! exchanged good or service.  Services are exchanged in non-dimensional values
//! (i.e. repairs per second), while goods may be exchanged either non-dimensionally,
//! or in mass-rate (kg/hr. etc.).
class WSF_EXPORT Tender : public WsfNamed
{
public:
   static bool LoadInstance(UtInput& aInput, Tender*& aTenderPtr);

   Tender();
   Tender(WsfStringId aTypeId,
          double      aQuantity  = 0.0,
          double      aRate      = 0.0, // default is instantaneously, if possible
          bool        aIsService = false);     // default is a commodity, not a service
   Tender* Clone() const;

   bool ProcessInput(UtInput& aInput);
   bool Initialize();

   void Null() { mQuantity = 0.0; }
   bool IsNull() const { return mQuantity == 0.0; }

   bool operator==(const Tender& aRhs) const;

   double Rate() const { return mRate; }
   bool   IsService() const { return mIsService; }
   bool   IsCommodity() const { return !mIsService; }
   double Interval() const { return (mRate == 0.0) ? 0.0 : mQuantity / mRate; }
   double Quantity() const { return mQuantity; }
   bool   IsCompatibleWith(const Tender& aRhs) const;

   void SetQuantity(double aValue) { SetQuantityP(aValue); }
   void SetRate(double aValue) { SetRateP(aValue); }

   // Note:  This command is not normally for end-user to use...
   void SetIsService(bool aValue);

   void   ConstrainQuantityTo(double aQuantity);
   void   ConstrainRateTo(double aRate);
   void   ConstrainTo(const Tender& aLimit);
   double Decrement(double aAmount);
   double Increment(double aAmount);

   const char* GetScriptClassName() const;

protected:
   void SetQuantityP(double aQuantity);
   void SetRateP(double aQuantity);

   double mQuantity;  // Max Quantity Requested or Offered.
   double mRate;      // Max Transactor Rate supported.
   bool   mIsService; // If true, is not a tangible commodity, but an action, such as "repair".

}; // end WsfExchange::Tender

class Result
{
public:
   enum Value
   {
      cPENDING            = 10,
      cIN_PROGRESS        = 11,
      cTIMED_OUT          = 12,
      cCANCELLED_REJECTED = 13,
      cPARTLY_SATISFIED   = 14,
      cFULLY_SATISFIED    = 15
   };

   Result(Value aStartValue = cPENDING)
      : mValue(aStartValue){};
   void        Set(Value aValue) { mValue = aValue; }
   Value       Get() const { return mValue; }
   bool        IsDone() const;
   void        Restart();
   std::string GetString() const;

protected:
   Value mValue;
};

//! Callback Query class.
class Query
{
public:
   Query(WsfSimulation* aSimulationPtr = nullptr);

   // Const 'Gets':
   bool IsOfferor() const { return mIsOfferor; }
   bool IsService() const { return mIsService; }
   bool IsDone() const { return mResult.IsDone(); }

   size_t       OwningIndex() const { return mOwningIndex; }
   unsigned int RequestId() const { return mRequestId; }
   size_t       RequestingIndex() const { return mRequestingIndex; }
   size_t       RespondingIndex() const { return mRespondingIndex; }

   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

   double OfferedQuantity() const { return mOfferedQuantity; }
   double OfferedRate() const { return mOfferedRate; }
   double DesiredQuantity() const { return mDesiredQuantity; }
   double DesiredRate() const { return mDesiredRate; }
   double NegotiatedQuantity() const { return mNegotiatedQuantity; }
   double NegotiatedRate() const { return mNegotiatedRate; }
   double NegotiatedInterval() const;

   std::string   GetResultString() const { return mResult.GetString(); }
   Result::Value GetResultValue() const { return mResult.Get(); }
   size_t        IntendedRecipientIndex() const;
   size_t        OppositeIndex() const;

   WsfStringId ItemId() const { return mItemId; }
   WsfStringId StateId() const { return mStateId; }
   WsfStringId EventId() const { return mEventId; }

   // Non-const 'Sets':
   void SetOwningIndex(size_t aValue);
   void SetRespondingIndex(size_t aValue);
   void SetResult(Result::Value aValue);
   void SetOfferedQuantity(double aValue);
   void SetDesiredQuantity(double aValue);
   void SetOfferedRate(double aValue);
   void SetDesiredRate(double aValue);
   void SetRequestId(unsigned int aValue);
   void SetNegotiatedQuantity(double aValue);
   void SetNegotiatedRate(double aValue);

   // Other methods:
   void ConstrainTo(double aQuantity);

   //! This will set the Query to an IsDone() State
   //! (TimedOut), and null all quantities.
   void FinalReset();
   //! This is used for an Offeror who will continue offering,
   // provided his quantity is not exhausted yet.
   void ContinuingReset(double aNewOfferQuantity);

   // For a given Query, these items are set once, and never changed:
   bool Initialize(WsfPlatform* aOwningPlatform, bool aIsOfferor, WsfStringId aCommodityId, bool aIsService);

   bool FormRequest(size_t       aOwningIndex,
                    size_t       aRequestingIndex,
                    size_t       aRespondingIndex,
                    double       aDesiredQuantity,
                    double       aDesiredRate,
                    unsigned int aRequestId,
                    WsfStringId  aItemId,
                    bool         aIsOfferor,
                    bool         aIsService);

   bool FormOffer(size_t       aOwningIndex,
                  size_t       aRequestingIndex,
                  size_t       aRespondingIndex,
                  double       aDesiredQuantity,
                  double       aDesiredRate,
                  double       aOfferedQuantity,
                  double       aOfferedRate,
                  unsigned int aRequestId,
                  WsfStringId  aItemId,
                  bool         aIsOfferor,
                  bool         aIsService);

   void SetState(WsfStringId aValue) { mStateId = aValue; }
   void SetEvent(WsfStringId aValue) { mEventId = aValue; }

   Tender GetTender() const;
   void   DebugDump(ut::log::MessageStream& aStream) const;

   // Returns the new state that should be, after the provided event triggers:
   WsfStringId StateMachine(WsfStringId aEventId) const;

   const char* GetScriptClassName() const;

private:
   WsfStringId    mItemId; // or service
   WsfStringId    mStateId;
   WsfStringId    mEventId;
   bool           mIsService;
   bool           mIsOfferor;   // the initiator of this event.
   size_t         mOwningIndex; // the initiator of this event.
   unsigned int   mRequestId;
   size_t         mRespondingIndex; // the index of the platform Suppling Product
   double         mOfferedQuantity;
   double         mOfferedRate;
   size_t         mRequestingIndex; // the index of the platform Receiving Product
   double         mDesiredQuantity;
   double         mDesiredRate;
   double         mNegotiatedQuantity;
   double         mNegotiatedRate;
   Result         mResult;
   WsfSimulation* mSimulationPtr{nullptr};
};

// ================================================================================
// ======================= Begin Platform Capabilities ============================
enum CapabilityType
{
   // This enumeration identifies what services the platform offers, if any.
   // (Compare this to the values in DisCapabilities.hpp.)
   cSUPPLY_PAYLOAD   = 0, // NOTE:  This name varies from the DIS standard
   cSUPPLY_FUEL      = 1,
   cVEHICLE_RECOVERY = 2,
   cVEHICLE_REPAIR   = 3,
   cSUPPLY_ADS_B     = 4, // Automatic Dependent Surveillance - Broadcast
   cNO_CAPABILITY    = 5  // enum hack.
};

void ModifyCapabilities(WsfPlatform* aPlatformPtr, unsigned int aOldCapability, unsigned int aNewCapability);

void SetCapabilityFlag(WsfPlatform* aPlatformPtr, CapabilityType aCapability, bool aValue);

bool IsCapable(const WsfPlatform* aPlatformPtr, CapabilityType aCapability);
bool IsCapable(unsigned int aCapabilityWord, CapabilityType aCapability);

void SetCapabilityFlag(unsigned int& aCapabilityWord, CapabilityType aCapability, bool aValue);

// Capability Translators:
unsigned int   CapabilityToBitNumber(CapabilityType aCapability);
CapabilityType BitNumberToCapability(unsigned int aBitNumber);
WsfStringId    CapabilityToNameId(CapabilityType aCapability);
CapabilityType NameIdToCapability(WsfStringId aCapabilityNameId);

// ======================= End Platform Capabilities ============================
// ==============================================================================


// ========================== CLASS CONTAINER ==================================
//! Container, which holds the quantity of tangible or non-tangible items that
//! are exchanged in a Transactor.
class WSF_EXPORT Container : public WsfNamed
{
public:
   static bool LoadInstance(UtInput& aInput, Container*& aContainerPtr);

   Container();
   Container(WsfStringId aNameId);

   bool ProcessInput(UtInput& aInput);
   bool Initialize();

   bool   IsFull() const;
   bool   IsEmpty() const;
   double CurrentQuantity() const { return mQuantity; }
   double MaximumQuantity() const { return mTender.Quantity(); }
   double QuantityNeeded() const;
   bool   IsCompatibleWith(const Tender& aTender) const;

   Tender TenderNeeded() const;
   Tender TenderRemaining() const;

   const Tender& MaximumTender() const { return mTender; }
   Tender&       MaximumTender() { return mTender; }

   bool   IsService() const { return mTender.IsService(); }
   bool   SetQuantity(double aValue);
   double Decrement(double aAmount);
   double Increment(double aAmount);

   const char* GetScriptClassName() const;

protected:
   Tender mTender;   // This indicates a maximum not-to-exceed capacity for the container.
   double mQuantity; // This represents a current amount in the container, constrained to mTender.mQuantity.

}; // end WsfExchange::Container

// ========================== CLASS Transactor =====================================
//! Transactor of goods or services.  Identifies the potential platforms involved
//! in the exchange.  A unique RequestId is held to distinguish this among a number
//! of Transactions that may be going on simultaneously.  Note that this Transactor
//! does not "own" the produce or service, as it is stored in the Container.  But
//! as a broker, he is allowed to commit and hold a particular quantity for the
//! transaction being considered at the moment.
class WSF_EXPORT Transactor : public WsfNamed
{
public:
   // Transactor States are enumerated above, at top of file.
   // For debugging.  Confirms state string is valid.
   static bool IsState(WsfStringId aState);
   static bool IsEvent(WsfStringId aEventId);

   static bool LoadInstance(UtInput& aInput, Transactor*& aInstancePtr);

   static unsigned int NextSerialRequestId();

   // Reset RequestId counter to zero.
   static void ResetNextSerialRequestId();

   // Constructors/Destructor
   Transactor();
   ~Transactor() override; // not virtual, as there are no derived classes.

   bool Initialize(double aSimTime, WsfExchangeProcessor* aProcessorPtr);

   WsfSimulation* GetSimulation() const;

   bool IsReady() const { return mIsEnabled && mQuery.StateId() == sSTATE_READY; }
   bool IsInProgress() const { return mIsEnabled && !mQuery.IsDone(); }
   bool ContinueToOffer() const { return mIsContinueToOffer; }
   bool AlwaysHonorRate() const { return mIsAlwaysHonorRate; }
   bool IsEnabled() const { return mIsEnabled; }
   bool IsOfferor() const { return mIsOfferor; }
   bool IsRequester() const { return !mIsOfferor; }

   bool IsHookedToFuel() const { return mIsHookedToFuel; }
   bool IsHookedToPayload() const { return mIsHookedToPayload; }
   bool IsExclusiveHook() const { return mIsExclusiveHook; }

   WsfStringId      ContainerNameId() const { return mContainerNameId; }
   Container*       ContainerPtr() { return mContainerPtr; }
   const Container* ContainerPtr() const { return mContainerPtr; }

   double TimeOutClockInterval() const { return mTimeOutClockInterval; }

   std::string OwningPlatformName() const;
   std::string OppositePlatformName() const;
   std::string RequestorPlatformName() const;
   std::string ResponderPlatformName() const;
   std::string MyFullName() const; // For debugging.

   bool RequestFrom(double aSimTime, const WsfPlatform* aSupplierPtr, const Tender& aTender);

   // Note that "Cancel" will terminate the transaction in progress,
   // if any.  To simultaneously disable the transactor from further
   // transactions, call Disable() instead.
   void Cancel(double aSimTime);
   bool DebugEnabled() const;
   bool IsService() const { return mContainerPtr->IsService(); }
   void DebugDump(double aSimTime, const Query& aQuery, ut::log::MessageStream& aMessageStream) const;

   void   SetIsOfferor(bool aValue) { mIsOfferor = aValue; }
   void   SetContinueToOffer(bool aValue) { mIsContinueToOffer = aValue; }
   void   SetIsHookedToPayload(bool aValue) { mIsHookedToPayload = aValue; }
   void   SetIsHookedToFuel(bool aValue) { mIsHookedToFuel = aValue; }
   void   SetTimeOutClockInterval(double aValue) { mTimeOutClockInterval = aValue; }
   void   SetProximityLimit(double aValue) { mProximityLimit = aValue; }
   double ProximityLimit() const { return mProximityLimit; }

   Query&       GetQuery() { return mQuery; }
   Query*       GetQueryPtr() { return &mQuery; }
   const Query& GetQuery() const { return mQuery; }

   bool ProcessInput(UtInput& aInput);

   // Callback to test for interest in this Transactor.
   // Return true =  I was impacted by this query... it was for me.
   bool ExchangeQueried(double aSimTime, const Query& aQuery);

   // bool ExchangeNegotiated(double       aSimTime,
   //                         const Query& aQuery);

   // Called by an event to kick the class into action:
   void TakeQueuedAction(double aSimTime, unsigned int aRequestId, bool aIsTimeOut);

   bool ReserveAll(double aSimTime);

   bool ReserveTender(double aSimTime, const Tender& aTender);

   bool ReserveFor(double aSimTime, const Tender& aTender, WsfPlatform* aReceiverPtr);

   void ResetUserAmounts();

   bool IsInConflictWith(const Transactor& aRhs) const;

   bool SetDesiredTender(const Tender& aTender);
   bool SetOfferedTender(const Tender& aTender);

   void Disable();

   const char* GetScriptClassName() const;

private:
   bool                ConsummateNow(double aSimTime);
   bool                RepairWasCompleted(double aSimTime);
   void                DetermineFinalResult(bool aWasTimedOut = false);
   bool                NegotiationComplete(double aSimTime, double aNegotiatedQuantity);
   bool                RequestFrom(double aSimTime, size_t aSupplierIndex);
   bool                Offer(double aSimTime, const Query& aQuery);
   bool                Accept(double aSimTime, const Query& aQuery);
   bool                Supply(double aSimTime);
   void                SetEnable(bool aValue);
   void                UnreserveFromContainer();
   static unsigned int sNextRequestId;
   bool                IsInProximityTo(size_t aOppositePlatIndex);

   // ============= Begin Sub-Class ===================
   class QueuedEvent : public WsfEvent
   {
      // There are only two possible events to Queue.
      // The first is a timeout, the second is a Consumate.
   public:
      QueuedEvent(double aSimTime, unsigned int aRequestId, size_t aPlatformIndex, Transactor* aTransactorPtr, bool aIsTimeOut);
      EventDisposition Execute() override;

   private:
      bool         mIsTimeOut; // else Consummate
      unsigned int mRequestId;
      size_t       mPlatIndex;
      Transactor*  mTransactorPtr;
   };
   // ============= End Sub-Class ===================

   WsfStringId           mContainerNameId;
   double                mStartTime;
   Container*            mContainerPtr;
   WsfExchangeProcessor* mExchangeProcPtr;
   bool                  mIsEnabled;
   bool                  mIsOfferor;         // Is the supplier of a commodity or service, else consumer.
   bool                  mIsContinueToOffer; // Once a transaction to supply is successful, stay on-line for others.
   bool                  mIsAlwaysHonorRate; // If true, we always stay
   bool                  mIsHookedToFuel;
   bool                  mIsHookedToPayload;
   bool                  mIsExclusiveHook; // License to totally own Fuel or Payload, and set quantity unilaterally.
   double mTimeOutClockInterval;           // How long do we wait for a response before we assume we will not get one?
   double mProximityLimit;                 // Optional.  If non-zero, will be checked prior to start of Transactor.
   double mReservedFromContainer;          // Amount TAKEN from container at start of transaction to supply.
   double mAttenuatedOfferAmount;          // Incrementally drawn from when offering...
   Query  mQuery;                          // Used in callbacks.
};                                         // end WsfExchange::Transactor

}; // end namespace WsfExchange

UT_MAP_CLASS_TO_SCRIPT_NAME(WsfExchange::Tender, "Tender")
UT_MAP_CLASS_TO_SCRIPT_NAME(WsfExchange::Container, "Container")
UT_MAP_CLASS_TO_SCRIPT_NAME(WsfExchange::Query, "Query")
UT_MAP_CLASS_TO_SCRIPT_NAME(WsfExchange::Transactor, "Transactor")

#endif
