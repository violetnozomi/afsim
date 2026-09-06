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

#include "WsfExchange.hpp"

#include <limits.h>

#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "WsfExchangeProcessor.hpp"
#include "WsfFuel.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfUtil.hpp"
#include "observer/WsfExchangeObserver.hpp"

namespace
{
const double      cNEARLY_ZERO = 0.00000001;
const std::string cUNKNOWN     = "<unknown>";

double MinThatIsNotZero(double a, double b)
{
   double result = std::max(a, b);
   if (a > 0.0 && a < result)
   {
      result = a;
   }
   if (b > 0.0 && b < result)
   {
      result = b;
   }
   return result;
}

double MinThatIsNotZero(double a, double b, double c)
{
   double result = std::max(a, std::max(b, c));
   if (a > 0.0 && a < result)
   {
      result = a;
   }
   if (b > 0.0 && b < result)
   {
      result = b;
   }
   if (c > 0.0 && c < result)
   {
      result = c;
   }
   return result;
}
} // namespace

namespace WsfExchange
{

// static
void InitializeStatics()
{
   if (sSTATE_READY.Empty())
   {
      // Enumerated Transactor States:
      sSTATE_READY      = "STATE_READY";
      sSTATE_REQUESTING = "STATE_REQUESTING";
      sSTATE_ACCEPTING  = "STATE_ACCEPTING";
      sSTATE_OFFERING   = "STATE_OFFERING";
      sSTATE_SUPPLYING  = "STATE_SUPPLYING";

      // Enumerated Transactor Events:
      sEVENT_REQUESTED           = "EVENT_REQUESTED";
      sEVENT_OFFERED             = "EVENT_OFFERED";
      sEVENT_ACCEPTED            = "EVENT_ACCEPTED";
      sEVENT_SUPPLIED            = "EVENT_SUPPLIED";
      sEVENT_RECEIVED            = "EVENT_RECEIVED";
      sEVENT_CANCELLED           = "EVENT_CANCELLED";
      sEVENT_OFFEROR_COMPLETED   = "EVENT_OFFEROR_COMPLETED";
      sEVENT_REQUESTOR_RESPONDED = "EVENT_REQUESTOR_RESPONDED";
   }
}

// =================================================================================================
// ==== Start Platform Capability ==== Start Platform Capability ==== Start Platform Capability ====
// static
void ModifyCapabilities(WsfPlatform* aPlatformPtr, unsigned int aOldCapability, unsigned int aNewCapability)
{
   // Traverse the bits and generate a callback for each bit that has changed:
   for (unsigned int i = 0; i != cNO_CAPABILITY; ++i)
   {
      CapabilityType capability   = BitNumberToCapability(i);
      WsfStringId    capabilityId = CapabilityToNameId(capability);
      bool           oldBool      = IsCapable(aOldCapability, capability);
      bool           newBool      = IsCapable(aNewCapability, capability);
      if (oldBool != newBool)
      {
         double simTime = aPlatformPtr->GetSimulation()->GetSimTime();
         WsfObserver::PlatformCapabilityChanged(aPlatformPtr->GetSimulation())(simTime, aPlatformPtr, capabilityId, newBool);
      }
   }
}

// =================================================================================================
void SetCapabilityFlag(WsfPlatform* aPlatformPtr, CapabilityType aCapability, bool aValue)
{
   unsigned int word = aPlatformPtr->GetCapabilities();
   SetCapabilityFlag(word, aCapability, aValue);
   aPlatformPtr->SetCapabilities(word);
}

// =================================================================================================
void SetCapabilityFlag(unsigned int& aCurrentCapability, CapabilityType aCapability, bool aValue)
{
   // We are only dealing with one bit at a time here, so no "protect" mask is needed.
   unsigned int bitNumber   = CapabilityToBitNumber(aCapability);
   unsigned int position    = std::min(std::max(0u, bitNumber), 32u);
   unsigned int endPosition = std::min(position + std::max(0u, 1u), 32u);
   unsigned int bitMask     = (WsfUtil::GenerateBitMask(endPosition - position) << position);
   aCurrentCapability &= ~(bitMask);           // zero
   aCurrentCapability |= (aValue << position); // fill
}

// =================================================================================================
bool IsCapable(unsigned int aCapabilityWord, CapabilityType aCapability)
{
   unsigned int bitNumber   = CapabilityToBitNumber(aCapability);
   unsigned int position    = std::min(std::max(0u, bitNumber), 32u);
   unsigned int endPosition = std::min(position + std::max(0u, 1u), 32u);
   // Generate the mask; shift to the correct position; and
   unsigned int retVal = ((aCapabilityWord >> position) & WsfUtil::GenerateBitMask(endPosition - position));
   return (retVal) ? true : false;
}

// =================================================================================================
unsigned int CapabilityToBitNumber(CapabilityType aCapability)
{
   switch (aCapability)
   {
   case (cSUPPLY_PAYLOAD):
      return 0;
   case (cSUPPLY_FUEL):
      return 1;
   case (cVEHICLE_RECOVERY):
      return 2;
   case (cVEHICLE_REPAIR):
      return 3;
   case (cSUPPLY_ADS_B):
      return 4;
   case (cNO_CAPABILITY):
      return 5;
   }
   return 5;
}

// =================================================================================================
CapabilityType BitNumberToCapability(unsigned int aBitNumber)
{
   if (aBitNumber == 0)
   {
      return cSUPPLY_PAYLOAD;
   }
   else if (aBitNumber == 1)
   {
      return cSUPPLY_FUEL;
   }
   else if (aBitNumber == 2)
   {
      return cVEHICLE_RECOVERY;
   }
   else if (aBitNumber == 3)
   {
      return cVEHICLE_REPAIR;
   }
   else if (aBitNumber == 4)
   {
      return cSUPPLY_ADS_B;
   }
   else
   {
      return cNO_CAPABILITY;
   }
}

// =================================================================================================
CapabilityType NameIdToCapability(WsfStringId aCapabilityNameId)
{
   if (aCapabilityNameId == WsfStringId("SUPPLY_PAYLOAD"))
   {
      return cSUPPLY_PAYLOAD;
   }
   else if (aCapabilityNameId == WsfStringId("SUPPLY_FUEL"))
   {
      return cSUPPLY_FUEL;
   }
   else if (aCapabilityNameId == WsfStringId("VEHICLE_RECOVERY"))
   {
      return cVEHICLE_RECOVERY;
   }
   else if (aCapabilityNameId == WsfStringId("VEHICLE_REPAIR"))
   {
      return cVEHICLE_REPAIR;
   }
   else if (aCapabilityNameId == WsfStringId("SUPPLY_ADS_B"))
   {
      return cSUPPLY_ADS_B;
   }
   else
   {
      return cNO_CAPABILITY;
   }
}

// =================================================================================================
// ===============================================================================
//! This is a convenience method to make the Capabilities record in the platform
//! more user-friendly, as the DIS-driven implementation requires bit-mapping
//! the capability bits.  This function abstracts away those unpleasant details.
bool IsCapable(const WsfPlatform* aPlatformPtr, CapabilityType aCapability)
{
   unsigned int word = aPlatformPtr->GetCapabilities();
   return IsCapable(word, aCapability);
}

// =================================================================================================
// =========================================================================
WsfStringId CapabilityToNameId(CapabilityType aCapability)
{
   WsfStringId result("NO_CAPABILITY");
   switch (aCapability)
   {
   case cSUPPLY_PAYLOAD:
      result.SetString("SUPPLY_PAYLOAD");
      break;
   case cSUPPLY_FUEL:
      result.SetString("SUPPLY_FUEL");
      break;
   case cVEHICLE_RECOVERY:
      result.SetString("VEHICLE_RECOVERY");
      break;
   case cVEHICLE_REPAIR:
      result.SetString("VEHICLE_REPAIR");
      break;
   case cSUPPLY_ADS_B:
      result.SetString("SUPPLY_ADS_B");
      break;
   case cNO_CAPABILITY:
      break;
   default:
      break;
   }
   return result;
}

// ==== End Platform Capability ==== End Platform Capability ==== End Platform Capability ====

// =================================================================================================
// ==== Begin Transaction Query ==== Begin Transaction Query ==== Begin Transaction Query =====

// =================================================================================================
std::string Result::GetString() const
{
   switch (mValue)
   {
   case cPENDING:
      return "Pending";
      break;
   case cIN_PROGRESS:
      return "InProgress";
      break;
   case cTIMED_OUT:
      return "TimedOut";
      break;
   case cCANCELLED_REJECTED:
      return "Rejected";
      break;
   case cPARTLY_SATISFIED:
      return "Partial_OK";
      break;
   case cFULLY_SATISFIED:
      return "Full_OK";
      break;
   default:
      return "Unknown";
      break;
   }
}

// =================================================================================================
Query::Query(WsfSimulation* aSimulationPtr)
   : mItemId(nullptr)
   , mStateId(sSTATE_READY)
   , mEventId(nullptr)
   , mIsService(false)
   , mIsOfferor(false)
   , mOwningIndex(0)
   , mRequestId(0)
   , mRespondingIndex(0)
   , mOfferedQuantity(0)
   , mOfferedRate(0)
   , mRequestingIndex(0)
   , mDesiredQuantity(0)
   , mDesiredRate(0)
   , mNegotiatedQuantity(0)
   , mNegotiatedRate(0)
   , mResult(Result::cTIMED_OUT)
   , mSimulationPtr(aSimulationPtr)
{
}

// =================================================================================================
// Here we have to tightly control encapsulation so that only
// one respondent can complete this transaction at a time!
void Query::SetResult(Result::Value aValue)
{
   mResult = aValue;
}

// =================================================================================================
bool Result::IsDone() const
{
   return !(mValue == cPENDING || mValue == cIN_PROGRESS);
}

// =================================================================================================
void Query::FinalReset()
{
   mStateId            = sSTATE_READY;
   mRequestId          = 0;
   mOfferedQuantity    = 0.0;
   mOfferedRate        = 0.0;
   mDesiredQuantity    = 0.0;
   mDesiredRate        = 0.0;
   mNegotiatedQuantity = 0.0;
   mNegotiatedRate     = 0.0;
   mResult.Set(Result::cTIMED_OUT);
}

// =================================================================================================
void Query::ContinuingReset(double aNewOfferQuantity)
{
   mResult             = Result::cPENDING;
   mStateId            = sSTATE_READY;
   mRequestId          = 0;
   mOfferedQuantity    = aNewOfferQuantity;
   mNegotiatedQuantity = aNewOfferQuantity;
   mNegotiatedRate     = 0.0;
   mDesiredQuantity    = 0.0;
   mDesiredRate        = 0.0;
}

// =================================================================================================
void Query::DebugDump(ut::log::MessageStream& aStream) const
{
   aStream << std::boolalpha;
   aStream.AddNote() << "Item: " << mItemId;
   aStream.AddNote() << "State: " << mStateId;
   aStream.AddNote() << "Event: " << mEventId;
   aStream.AddNote() << "Is Service: " << mIsService;
   aStream.AddNote() << "Is Offerer: " << mIsOfferor;
   aStream.AddNote() << "Request ID: " << mRequestId;
   aStream.AddNote() << "Offered Qty: " << mOfferedQuantity;
   aStream.AddNote() << "Offered Rate: " << mOfferedRate;
   aStream.AddNote() << "Desired Qty: " << mDesiredQuantity;
   aStream.AddNote() << "Desired Rate: " << mDesiredRate;
   aStream.AddNote() << "Negotiated Qty: " << mNegotiatedQuantity;
   aStream.AddNote() << "Negotiated Rate: " << mNegotiatedRate;
   aStream.AddNote() << "Result: " << mResult.GetString();
   aStream.AddNote() << "Owning Index: " << mOwningIndex;
   aStream.AddNote() << "Requesting Index: " << mRequestingIndex;
   aStream.AddNote() << "Responding Index: " << mRespondingIndex;
}

// =================================================================================================
void Query::ConstrainTo(double aQuantity)
{
   if (mNegotiatedQuantity > aQuantity)
   {
      mNegotiatedQuantity = aQuantity;
   }
}

// =================================================================================================
size_t Query::OppositeIndex() const
{
   return (mIsOfferor) ? mRequestingIndex : mRespondingIndex;
}

// =================================================================================================
void Query::SetOwningIndex(size_t aValue)
{
   mOwningIndex = aValue;
}

// =================================================================================================
void Query::SetRespondingIndex(size_t aValue)
{
   mRespondingIndex = aValue;
}

// =================================================================================================
void Query::SetNegotiatedQuantity(double aValue)
{
   mNegotiatedQuantity = aValue;
}

// =================================================================================================
void Query::SetOfferedQuantity(double aValue)
{
   mOfferedQuantity = aValue;
}

// =================================================================================================
void Query::SetDesiredQuantity(double aValue)
{
   mDesiredQuantity = aValue;
}

// =================================================================================================
void Query::SetOfferedRate(double aValue)
{
   mOfferedRate = aValue;
}

// =================================================================================================
void Query::SetNegotiatedRate(double aValue)
{
   mNegotiatedRate = aValue;
}

// =================================================================================================
void Query::SetRequestId(unsigned int aValue)
{
   mRequestId = aValue;
};

// =================================================================================================
double Query::NegotiatedInterval() const
{
   if (mNegotiatedRate == 0.0)
   {
      return 0.0;
   }
   return mNegotiatedQuantity / mNegotiatedRate;
};

// =================================================================================================
bool Query::Initialize(WsfPlatform* aOwningPlatform, bool aIsOfferor, WsfStringId aCommodityId, bool aIsService)
{
   assert(mResult.IsDone());
   mOwningIndex = aOwningPlatform->GetIndex();
   mIsOfferor   = aIsOfferor;
   mItemId      = aCommodityId;
   mIsService   = aIsService;

   if (mIsOfferor)
   {
      mRespondingIndex = mOwningIndex;
   }
   else
   {
      mRequestingIndex = mOwningIndex;
   }

   mSimulationPtr = aOwningPlatform->GetSimulation();

   return true;
}

// =================================================================================================
Tender Query::GetTender() const
{
   // NOTE:  Rate is currently always returned as zero.  Revisit later.
   double quantity = (mIsOfferor) ? mOfferedQuantity : mDesiredQuantity;
   double rate     = (mIsOfferor) ? mOfferedRate : mDesiredRate;
   return Tender(mItemId, quantity, rate, mIsService);
}

// =================================================================================================
bool Query::FormRequest(size_t       aOwningIndex,
                        size_t       aRequestingIndex,
                        size_t       aRespondingIndex,
                        double       aDesiredQuantity,
                        double       aDesiredRate,
                        unsigned int aRequestId,
                        WsfStringId  aItemId,
                        bool         aIsOfferor,
                        bool         aIsService)
{
   mOwningIndex        = aOwningIndex;
   mStateId            = sSTATE_REQUESTING;
   mEventId            = sEVENT_REQUESTED;
   mRequestId          = aRequestId;
   mRespondingIndex    = aRespondingIndex;
   mRequestingIndex    = aRequestingIndex;
   mOfferedQuantity    = aDesiredQuantity; // placeholder only
   mDesiredQuantity    = aDesiredQuantity;
   mNegotiatedQuantity = aDesiredQuantity; // placeholder only
   mOfferedRate        = aDesiredRate;     // placeholder only
   mDesiredRate        = aDesiredRate;
   mNegotiatedRate     = aDesiredRate; // placeholder only
   mIsOfferor          = aIsOfferor;
   mItemId             = aItemId;
   mIsService          = aIsService;
   return true;
}

// =================================================================================================
bool Query::FormOffer(size_t       aOwningIndex,
                      size_t       aRequestingIndex,
                      size_t       aRespondingIndex,
                      double       aDesiredQuantity,
                      double       aDesiredRate,
                      double       aOfferedQuantity,
                      double       aOfferedRate,
                      unsigned int aRequestId,
                      WsfStringId  aItemId,
                      bool         aIsOfferor,
                      bool         aIsService)
{
   // assert(mOfferedQuantity > 0.0);
   assert(aDesiredQuantity > 0.0);
   mOwningIndex        = aOwningIndex;
   mStateId            = sSTATE_OFFERING;
   mEventId            = sEVENT_OFFERED;
   mRequestId          = aRequestId;
   mOfferedQuantity    = aOfferedQuantity;
   mOfferedRate        = aOfferedRate;
   mDesiredQuantity    = aDesiredQuantity;
   mDesiredRate        = aDesiredRate;
   mNegotiatedQuantity = mOfferedQuantity;
   mRequestingIndex    = aRequestingIndex;
   mRespondingIndex    = aRespondingIndex;
   mItemId             = aItemId;
   mIsOfferor          = aIsOfferor;
   mIsService          = aIsService;
   mNegotiatedRate     = MinThatIsNotZero(mDesiredRate, mOfferedRate);
   assert(mOwningIndex != 0);
   return true;
}

// =================================================================================================
WsfStringId Query::StateMachine(WsfStringId aEventId) const
{
   WsfStringId newStateId = mStateId; // default if none below apply...

   if ((aEventId == sEVENT_REQUESTED) && (mStateId == sSTATE_READY))
   {
      // The REQUEST is incoming, we are going to OFFER
      assert(mIsOfferor);
      newStateId = sSTATE_OFFERING;
   }
   else if ((aEventId == sEVENT_OFFERED) && (mStateId == sSTATE_REQUESTING))
   {
      // An OFFER is incoming, we are going to ACCEPT
      assert(!mIsOfferor);
      newStateId = sSTATE_ACCEPTING;
   }
   else if ((aEventId == sEVENT_ACCEPTED) && (mStateId == sSTATE_OFFERING))
   {
      // An ACCEPT is incoming, we are going to SUPPLY
      // (I was offering goods to a client.  He has agreed to accept them.)
      assert(mIsOfferor);
      newStateId = sSTATE_SUPPLYING;
   }
   else if ((aEventId == sEVENT_CANCELLED) && (mStateId == sSTATE_ACCEPTING))
   {
      assert(!mIsOfferor);
      newStateId = sSTATE_READY;
   }
   else if ((aEventId == sEVENT_SUPPLIED) && (mStateId == sSTATE_ACCEPTING))
   {
      // Our supplier expressed intent to provide what we need.
      // Our contract is complete, so wait for it to finish.
      assert(!mIsOfferor);
      newStateId = sSTATE_READY;
   }
   else if ((aEventId == sEVENT_RECEIVED) && (mStateId == sSTATE_SUPPLYING))
   {
      // Our recipient acknowledged the receipt of goods/services.
      // Our contract is complete, so wait for it to finish.
      assert(mIsOfferor);
      newStateId = sSTATE_READY;
   }
   else if ((aEventId == sEVENT_RECEIVED) && (mStateId == sSTATE_OFFERING))
   {
      // The client that we supplied to has Acknowledged receipt,
      // so we are free to return to READY.
      newStateId = sSTATE_READY;
   }
   else if ((aEventId == sEVENT_OFFEROR_COMPLETED) && (mStateId == sSTATE_REQUESTING))
   {
      // Our recipient says he repaired us.
      newStateId = sSTATE_READY;
   }
   else if ((aEventId == sEVENT_OFFEROR_COMPLETED) && (mStateId == sSTATE_ACCEPTING))
   {
      // Our offeror says he repaired us.
      newStateId = sSTATE_READY;
   }
   else if ((aEventId == sEVENT_REQUESTOR_RESPONDED) && (mStateId == sSTATE_SUPPLYING))
   {
      // The client that we repaired has Acknowledged our repair,
      // so we are free to return to READY.
      newStateId = sSTATE_READY;
   }

   return newStateId;
}

// ===============================================================================
// ========================== BEGIN CLASS TENDER =================================

// =========================================================================
// static
bool Tender::LoadInstance(UtInput& aInput, Tender*& aTenderPtr)
{
   bool        loaded  = false;
   std::string command = aInput.GetCommand();
   if ((command == "commodity") || (command == "service"))
   {
      loaded = true;
      delete aTenderPtr;

      std::string name;
      aInput.ReadValue(name);

      aTenderPtr = new Tender(name);
      aTenderPtr->SetIsService(command == "service");

      UtInputBlock block(aInput, "end_" + command);
      while (block.ReadCommand())
      {
         if (!aTenderPtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return loaded;
}

// =========================================================================
Tender::Tender()
   : WsfNamed()
   , mQuantity(0.0)
   , mRate(0.0)
   , mIsService(false)
{
}

// =========================================================================
Tender::Tender(WsfStringId aId, double aQuantity, double aRate, bool aIsService)
   : WsfNamed(aId.GetString())
   , mQuantity(aQuantity)
   , mRate(aRate)
   , mIsService(aIsService)
{
}

// =========================================================================
bool Tender::operator==(const Tender& aRhs) const
{
   // Two Tenders are exactly the same if they have the same
   // Quantity, Rate, and are "conpatible".
   return mQuantity == aRhs.mQuantity && mRate == aRhs.mRate && IsCompatibleWith(aRhs);
}

// =================================================================================================
Tender* Tender::Clone() const
{
   return new Tender(*this);
}

// =========================================================================
bool Tender::IsCompatibleWith(const Tender& aRhs) const
{
   // Two Tenders are compatible if they are of the same NameId and IsService flag.
   return GetNameId() == aRhs.GetNameId() && mIsService == aRhs.mIsService;
}

// =========================================================================
bool Tender::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if ((command == "quantity") || (command == "maximum_quantity"))
   {
      aInput.ReadValue(mQuantity);
      aInput.ValueGreaterOrEqual(mQuantity, 0.0);
   }
   else if (IsCommodity() && ((command == "mass_quantity") || (command == "maximum_mass_quantity")))
   {
      aInput.ReadValueOfType(mQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mQuantity, 0.0);
   }
   else if (command == "rate")
   {
      aInput.ReadValue(mRate);
      aInput.ValueGreaterOrEqual(mRate, 0.0);
   }
   else if (IsService() && ((command == "service_interval") || (command == "minimum_service_interval")))
   {
      double duration;
      aInput.ReadValueOfType(duration, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(duration, 0.0);
      mRate = 0.0;
      if (duration > 0.0)
      {
         // Translate to "services per second"
         mRate = 1.0 / duration;
      }
   }
   else if (IsCommodity() && ((command == "mass_rate") || (command == "maximum_mass_rate")))
   {
      aInput.ReadValueOfType(mRate, UtInput::cMASS_TRANSFER);
      aInput.ValueGreaterOrEqual(mRate, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =========================================================================
void Tender::SetQuantityP(double aQuantity)
{
   mQuantity = aQuantity;
   if (mQuantity < 0.0)
   {
      mQuantity = 0.0;
   }
}

// =========================================================================
void Tender::SetRateP(double aRate)
{
   mRate = aRate;
   if (mRate < 0.0)
   {
      mRate = 0.0;
   }
}

// =========================================================================
void Tender::SetIsService(bool aValue)
{
   mIsService = aValue;
}

// =========================================================================
double Tender::Decrement(double aAmount)
{
   assert(aAmount >= 0.0);
   double actualDecrement = aAmount;
   if (actualDecrement > mQuantity)
   {
      actualDecrement = mQuantity;
   }
   mQuantity -= actualDecrement;
   if (mQuantity < 0.0)
   {
      mQuantity = 0.0;
   }
   return actualDecrement;
}

// =========================================================================
double Tender::Increment(double aAmount)
{
   // There is no failure mode here, so return same result.
   assert(aAmount >= 0.0);
   mQuantity += aAmount;
   return aAmount;
}

// =========================================================================
void Tender::ConstrainTo(const Tender& aLimit)
{
   // If one commodity or service is named the same as the other,
   // then constraining means limiting to BOTH quantity and rate limits
   assert(GetNameId() == aLimit.GetNameId());

   ConstrainQuantityTo(aLimit.mQuantity);

   // Choose the slowest non-zero rate between the two.
   // (If both are zero, then accept zero, which actually means "instantaneously".)
   double largest  = (aLimit.mRate > mRate) ? aLimit.mRate : mRate;
   double smallest = (aLimit.mRate < mRate) ? aLimit.mRate : mRate;

   if (smallest > 0.0)
   {
      ConstrainRateTo(smallest);
   }
   else if (largest > 0.0)
   {
      ConstrainRateTo(largest);
   }
}

// =========================================================================
void Tender::ConstrainQuantityTo(double aQuantity)
{
   if (aQuantity < mQuantity)
   {
      SetQuantityP(aQuantity);
   }
}

// =========================================================================
void Tender::ConstrainRateTo(double aRate)
{
   if (aRate < mRate)
   {
      SetRateP(aRate);
   }
}

// ========================== CLASS CONTAINER =================================

// =========================================================================
// static
bool Container::LoadInstance(UtInput& aInput, Container*& aContainerPtr)
{
   bool        loaded  = false;
   std::string command = aInput.GetCommand();
   if (command == "container")
   {
      loaded = true;
      delete aContainerPtr;

      std::string name;
      aInput.ReadValue(name);

      aContainerPtr = new Container(name);

      UtInputBlock block(aInput, "end_" + command);
      while (block.ReadCommand())
      {
         if (!aContainerPtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return loaded;
}

// =========================================================================
Container::Container()
   : WsfNamed()
   , mTender()
   , mQuantity(0.0)
{
}

// =========================================================================
Container::Container(WsfStringId aNameId)
   : WsfNamed(aNameId.GetString())
   , mTender()
   , mQuantity(0.0)
{
}

// =========================================================================
bool Container::IsCompatibleWith(const Tender& aTender) const
{
   return mTender.IsCompatibleWith(aTender);
}

// =========================================================================
Tender Container::TenderNeeded() const
{
   Tender rv = mTender;
   rv.SetQuantity(mTender.Quantity() - mQuantity);
   return rv;
}

// =========================================================================
Tender Container::TenderRemaining() const
{
   Tender rv = mTender;
   rv.SetQuantity(mQuantity);
   return rv;
}

// =========================================================================
bool Container::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   Tender*     tPtr      = nullptr;

   if (command == "initial_quantity")
   {
      aInput.ReadValue(mQuantity);
      aInput.ValueGreaterOrEqual(mQuantity, 0.0);
   }
   else if (command == "initial_mass_quantity")
   {
      aInput.ReadValueOfType(mQuantity, UtInput::cMASS);
   }
   else if (Tender::LoadInstance(aInput, tPtr))
   {
      mTender = *tPtr;
      delete tPtr;
      tPtr = nullptr;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =========================================================================
bool Container::Initialize()
{
   // This is just a constrain operation.
   // There is no failure mode.
   if (mQuantity > mTender.Quantity())
   {
      mQuantity = mTender.Quantity();
   }
   if (mQuantity < 0.0)
   {
      mQuantity = 0.0;
   }
   return true;
}

// =========================================================================
double Container::Increment(double aAmount)
{
   assert(aAmount >= 0.0);
   double actualIncr = aAmount;
   double limit      = mTender.Quantity();
   if (mQuantity + aAmount > limit)
   {
      actualIncr = limit - mQuantity;
   }
   mQuantity += actualIncr;
   if (mQuantity > limit)
   {
      mQuantity = limit;
   }
   return actualIncr;
}

// =========================================================================
double Container::Decrement(double aAmount)
{
   assert(aAmount >= 0.0);
   double actualDecr = aAmount;
   // double savedQuantity = mQuantity;
   if (mQuantity < aAmount)
   {
      actualDecr = mQuantity;
   }
   mQuantity -= actualDecr;
   if (mQuantity < 0.0)
   {
      mQuantity = 0.0;
   }

   return actualDecr;
}

// =========================================================================
bool Container::SetQuantity(double aValue)
{
   if ((aValue >= 0.0) && (aValue <= mTender.Quantity()))
   {
      mQuantity = aValue;
      return true;
   }
   return false;
}

// =========================================================================
double Container::QuantityNeeded() const
{
   return mTender.Quantity() - mQuantity;
}

// =========================================================================
bool Container::IsFull() const
{
   double denom = mTender.Quantity();
   if (denom <= cNEARLY_ZERO)
   {
      return false;
   }
   double fillRatio = mQuantity / denom;
   return fabs(fillRatio - 1.0) <= cNEARLY_ZERO;
}

// =========================================================================
bool Container::IsEmpty() const
{
   double denom = mTender.Quantity();
   if (denom <= cNEARLY_ZERO)
   {
      return false;
   }
   double fillRatio = mQuantity / denom;
   return fabs(fillRatio) <= cNEARLY_ZERO;
}

// ========================== BEGIN CLASS Transactor =================================

// =========================================================================
// static
unsigned int Transactor::NextSerialRequestId()
{
   return ++sNextRequestId;
}

// =========================================================================
// static private
unsigned int Transactor::sNextRequestId = 0;

// =========================================================================
Transactor::Transactor()
   : mContainerNameId()
   , mStartTime(-1.0)
   , // Intentionally bogus.
   mContainerPtr(nullptr)
   , mExchangeProcPtr(nullptr)
   , mIsEnabled(false)
   , mIsOfferor(false)
   , mIsContinueToOffer(false)
   , mIsAlwaysHonorRate(false)
   , mIsHookedToFuel(false)
   , mIsHookedToPayload(false)
   , mIsExclusiveHook(false)
   , mTimeOutClockInterval(10.0)
   , mProximityLimit(0.0)
   , mReservedFromContainer(0.0)
   , mAttenuatedOfferAmount(0.0)
   , mQuery()
{
}

// =========================================================================
// static
bool Transactor::IsEvent(WsfStringId aEventId)
{
   return aEventId == sEVENT_REQUESTED || aEventId == sEVENT_OFFERED || aEventId == sEVENT_ACCEPTED ||
          aEventId == sEVENT_SUPPLIED || aEventId == sEVENT_RECEIVED || aEventId == sEVENT_CANCELLED ||
          aEventId == sEVENT_OFFEROR_COMPLETED || aEventId == sEVENT_REQUESTOR_RESPONDED;
}

// =========================================================================
// static
void Transactor::ResetNextSerialRequestId()
{
   sNextRequestId = 0;
}

// =========================================================================
bool Transactor::SetOfferedTender(const Tender& aTender)
{
   // First, if the Transactor is not an Offeror,
   // and currently "ready", fail.
   if (!mIsOfferor && (GetQuery().StateId() != sSTATE_READY))
   {
      return false;
   }

   // Second, if the Tender is not compatible with the Container,
   // or if the quantity is zero, fail.
   if ((mContainerPtr == nullptr) || (!mContainerPtr->MaximumTender().IsCompatibleWith(aTender)) ||
       (aTender.Quantity() <= 0.0))
   {
      return false;
   }

   // Save off what we start with in terms of an offer, so that
   // as transactions are successful, each time we will re-offer
   // what has not yet been supplied (if this option is enabled).
   mAttenuatedOfferAmount = aTender.Quantity();

   // Now set the quantity and rate of offer...
   GetQuery().SetOfferedQuantity(aTender.Quantity());
   GetQuery().SetNegotiatedQuantity(aTender.Quantity()); // temporary, should get overwritten.

   // The Rate of Offer may be defaulted to zero, which means "instantaneously".
   // Due to DIS-isms, there is no rate communicated across DIS, so in that case,
   // the other side is always going to be claiming zero rate.  The negotiated
   // rate will not be known until later, when the two are compared.  If both are
   // non-zero, constrain rate to lesser of this and Container Rate.
   double rate = MinThatIsNotZero(mContainerPtr->MaximumTender().Rate(), aTender.Rate());

   GetQuery().SetOfferedRate(rate);
   GetQuery().SetNegotiatedRate(rate);

   return true;
}

// =========================================================================
bool Transactor::SetDesiredTender(const Tender& aTender)
{
   // First, if the Transactor is not currently "ready", fail.
   if (mIsOfferor || (GetQuery().StateId() != sSTATE_READY))
   {
      return false;
   }

   // Second, if the Tender is not compatible with the Container, fail.
   if ((mContainerPtr == nullptr) || (!mContainerPtr->MaximumTender().IsCompatibleWith(aTender)) ||
       (aTender.Quantity() <= 0.0))
   {
      return false;
   }

   GetQuery().SetDesiredQuantity(aTender.Quantity());

   return true;
}

// =========================================================================
bool Transactor::IsInConflictWith(const Transactor& aRhs) const
{
   // (This is a test done just before adding a new Transactor into a collection.)
   // Two transactors are in conflict if:  A) they have the same name, or B) they
   // transact the same thing and in the same _direction_ into the same container.
   if (GetNameId() == aRhs.GetNameId())
   {
      return true;
   }
   bool sameDirection = mIsOfferor == aRhs.mIsOfferor;
   bool sameContainer = mContainerNameId == aRhs.mContainerNameId;
   return sameDirection && sameContainer;
}

// =========================================================================
void Transactor::DebugDump(double aSimTime, const Query& aQuery, ut::log::MessageStream& aMessageStream) const
{
   auto& out = aMessageStream;
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "Transactor: " << MyFullName();
   out.AddNote() << "Is Enabled: " << mIsEnabled;
   out.AddNote() << "Container: " << mContainerNameId;
   out.AddNote() << "Available: " << mContainerPtr->CurrentQuantity();
   out.AddNote() << "Need: " << mContainerPtr->QuantityNeeded();

   { // RAII block
      auto myQueryNote = out.AddNote() << "My Query: ";
      mQuery.DebugDump(myQueryNote);
   }
   { // RAII block
      auto hisQueryNote = out.AddNote() << "His Query: ";
      aQuery.DebugDump(hisQueryNote);
   }
}

// =========================================================================
// static
bool Transactor::IsState(WsfStringId aState)
{
   return aState == sSTATE_READY || aState == sSTATE_OFFERING || aState == sSTATE_REQUESTING ||
          aState == sSTATE_ACCEPTING || aState == sSTATE_SUPPLYING;
}

// =========================================================================
// virtual
Transactor::~Transactor()
{
   // If we are being destructed, and have not yet been Disabled, do it now.
   // SetEnable(false);
}

// =========================================================================
// static
bool Transactor::LoadInstance(UtInput& aInput, Transactor*& aTransactorPtr)
{
   if (aInput.GetCommand() == "transactor")
   {
      UtInputBlock inputBlock(aInput);
      if (aTransactorPtr != nullptr)
      {
         delete aTransactorPtr;
      }
      aTransactorPtr = new Transactor();

      std::string name;
      inputBlock.GetInput().ReadValue(name);
      aTransactorPtr->SetName(name);

      while (inputBlock.ReadCommand())
      {
         UtInput& input = inputBlock.GetInput();
         if (!aTransactorPtr->ProcessInput(input))
         {
            throw UtInput::BadValue(input, "Unknown command " + input.GetCommand() + " in tranasactor block.");
         }
      }
      return true;
   }
   return false;
}

// =========================================================================
bool Transactor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if ((command == "offeror") || (command == "is_offeror") || (command == "supplier") || (command == "is_supplier"))
   {
      mIsOfferor = true;
   }
   else if ((command == "requestor") || (command == "is_requestor") || (command == "receiver") ||
            (command == "is_receiver"))
   {
      mIsOfferor = false;
   }
   else if (command == "continue_to_offer")
   {
      aInput.ReadValue(mIsContinueToOffer);
   }
   else if (command == "always_honor_rate")
   {
      aInput.ReadValue(mIsAlwaysHonorRate);
   }
   else if (command == "hook_to_fuel")
   {
      aInput.ReadValue(mIsHookedToFuel);
      if (mIsHookedToPayload && mIsHookedToFuel)
      {
         throw UtInput::BadValue(aInput, "Cannot hook a Transactor to both fuel and payload simultaneously.");
      }
   }
   else if (command == "hook_to_payload")
   {
      aInput.ReadValue(mIsHookedToPayload);
      if (mIsHookedToFuel && mIsHookedToPayload)
      {
         throw UtInput::BadValue(aInput, "Cannot hook a Transactor to both fuel and payload simultaneously.");
      }
   }
   else if ((command == "exclusive_hook_to_payload") || (command == "exclusive_hook_to_fuel"))
   {
      aInput.ReadValue(mIsExclusiveHook);
   }
   else if (command == "proximity_limit")
   {
      aInput.ReadValueOfType(mProximityLimit, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mProximityLimit, 0.0);
   }
   else if (command == "container_name")
   {
      std::string name;
      aInput.ReadValue(name);
      mContainerNameId.SetString(name);
   }
   else if (command == "time_out_clock_interval")
   {
      aInput.ReadValueOfType(mTimeOutClockInterval, UtInput::cTIME);
      aInput.ValueGreater(mTimeOutClockInterval, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =========================================================================
void Transactor::Disable()
{
   double simTime = mExchangeProcPtr->GetSimulation()->GetSimTime();
   ConsummateNow(simTime);
   SetEnable(false);
}

// =========================================================================
void Transactor::SetEnable(bool aValue)
{
   if (aValue != mIsEnabled)
   {
      if (DebugEnabled())
      {
         if (aValue)
         {
            auto out = ut::log::debug() << "Transactor enabled.";
            out.AddNote() << "Transactor: " << MyFullName();
         }
         else
         {
            auto out = ut::log::debug() << "Transactor disabled.";
            out.AddNote() << "Transactor: " << MyFullName();
         }
      }

      if (mIsOfferor)
      {
         CapabilityType cap = mExchangeProcPtr->GetCapabilityOf(mContainerPtr->MaximumTender().GetNameId());
         if (cap != cNO_CAPABILITY)
         {
            // Take the existing platform capabilities word, modify to set
            // the desired capability bits, and then set it back into the platform
            unsigned int capabilityWord = mExchangeProcPtr->GetPlatform()->GetCapabilities();
            SetCapabilityFlag(capabilityWord, cap, aValue);
            mExchangeProcPtr->GetPlatform()->SetCapabilities(capabilityWord);
         }
      }

      mIsEnabled = aValue;
   }
}

// =========================================================================
std::string Transactor::OwningPlatformName() const
{
   std::string  result = cUNKNOWN;
   WsfPlatform* pPtr   = mExchangeProcPtr->GetSimulation()->GetPlatformByIndex(GetQuery().OwningIndex());
   if (pPtr != nullptr)
   {
      result = pPtr->GetName();
   }
   return result;
}

// =========================================================================
std::string Transactor::OppositePlatformName() const
{
   std::string  result = cUNKNOWN;
   WsfPlatform* pPtr   = mExchangeProcPtr->GetSimulation()->GetPlatformByIndex(GetQuery().OppositeIndex());
   if (pPtr != nullptr)
   {
      result = pPtr->GetName();
   }
   return result;
}

// =========================================================================
std::string Transactor::RequestorPlatformName() const
{
   std::string  result = cUNKNOWN;
   WsfPlatform* pPtr   = mExchangeProcPtr->GetSimulation()->GetPlatformByIndex(GetQuery().RequestingIndex());
   if (pPtr != nullptr)
   {
      result = pPtr->GetName();
   }
   return result;
}

// =========================================================================
std::string Transactor::ResponderPlatformName() const
{
   std::string  result = cUNKNOWN;
   WsfPlatform* pPtr   = mExchangeProcPtr->GetSimulation()->GetPlatformByIndex(GetQuery().RespondingIndex());
   if (pPtr != nullptr)
   {
      result = pPtr->GetName();
   }
   return result;
}

// =========================================================================
std::string Transactor::MyFullName() const
{
   return mExchangeProcPtr->GetPlatform()->GetName() + ":" + mExchangeProcPtr->GetName() + ":" + GetName();
}

// =========================================================================
bool Transactor::IsInProximityTo(size_t aOtherPlatIndex)
{
   // SHortcut proximity checks if flag is so-enabled...
   if (mExchangeProcPtr->IgnoreAllProximityChecks())
   {
      return true;
   }

   if (mProximityLimit <= 0.0)
   {
      // If we did not set a proximity limit, then ignore this test.
      return true;
   }

   // If he does not exist, then fail proximity test.
   WsfPlatform* otherPlatPtr = mExchangeProcPtr->GetSimulation()->GetPlatformByIndex(aOtherPlatIndex);
   if (otherPlatPtr == nullptr)
   {
      return false;
   }

   // If I do not exist (not likely the case), then fail proximity test.
   WsfPlatform* myPlatPtr = mExchangeProcPtr->GetSimulation()->GetPlatformByIndex(GetQuery().OwningIndex());
   if (myPlatPtr == nullptr)
   {
      // Its just good practice to check for this.
      return false;
   }

   // Two valid platforms.  Now check proximity:
   double myLocWCS[3];
   myPlatPtr->GetLocationWCS(myLocWCS);

   double hisLocWCS[3];
   otherPlatPtr->GetLocationWCS(hisLocWCS);

   double diff[3];
   UtVec3d::Subtract(diff, myLocWCS, hisLocWCS);

   double distance = UtVec3d::Magnitude(diff);

   // Return the fact that he is or is not close enough to interact with.
   return distance <= mProximityLimit;
}

// =========================================================================
bool Transactor::ReserveAll(double aSimTime)
{
   // if (mContainerPtr == nullptr) { return false; }
   return ReserveTender(aSimTime, mContainerPtr->TenderRemaining());
}

// =========================================================================
bool Transactor::ReserveTender(double aSimTime, const Tender& aTender)
{
   // Since ReserveAll() and ReserveTender() may be called by script,
   // we have to be quite robust in assuring that this is successful
   // only when appropriate, and so error check accordingly.

   if ((mQuery.StateId() != sSTATE_READY) || (!IsOfferor()) || (aTender.Quantity() == 0.0) || (!aTender.GetNameId()) ||
       (!mContainerPtr->IsCompatibleWith(aTender)))
   {
      return false;
   }
   assert(mQuery.GetResultValue() == Result::cTIMED_OUT);

   bool isSet = SetOfferedTender(aTender);
   if (isSet)
   {
      SetEnable(true);
      mQuery.SetResult(Result::cPENDING);
   }

   return isSet;
}

// =========================================================================
bool Transactor::Initialize(double aSimTime, WsfExchangeProcessor* aProcessorPtr)
{
   mExchangeProcPtr = aProcessorPtr;
   if (mExchangeProcPtr == nullptr)
   {
      ut::log::error() << "No platform was provided to transactor.";

      return false;
   }

   mContainerPtr = mExchangeProcPtr->FindContainer(mContainerNameId);
   if (mContainerPtr == nullptr)
   {
      auto out = ut::log::error() << "Transactor must be given a named container to use.";
      out.AddNote() << "Transactor: " << MyFullName();
      out.AddNote() << "Use the 'container_name' command to resolve this issue.";
      return false;
   }

   if (mIsHookedToFuel)
   {
      if (IsService())
      {
         auto out = ut::log::error() << "It is non-sensical to hook a 'service' to fuel mass.";
         out.AddNote() << "Transactor: " << MyFullName();
         return false;
      }

      // If this transactor is to be hooked to the platform fuel, then we will constrain the
      // the user-provided maximum value (if any) with the values extracted from the fuel in the platform.
      double   containerMax     = mContainerPtr->MaximumTender().Quantity();
      double   containerCurrent = mContainerPtr->CurrentQuantity();
      WsfFuel* fPtr             = mExchangeProcPtr->GetPlatform()->GetComponent<WsfFuel>();
      bool     useFuel          = (fPtr != nullptr);

      if (mIsExclusiveHook)
      {
         if (useFuel)
         {
            // We own the fuel.  Force it to our container's max capacity and current capacity.
            fPtr->SetMaximumQuantity(containerMax);
            fPtr->SetInitialQuantity(containerCurrent);
            fPtr->SetQuantityRemaining(containerCurrent);
         }
         else
         {
            mExchangeProcPtr->GetPlatform()->SetFuelMass(containerCurrent);
         }
      }
      else
      {
         if (useFuel)
         {
            double fuelMax = fPtr->GetMaximumQuantity();

            if ((containerMax <= 0.0) || (containerMax > fuelMax))
            {
               mContainerPtr->MaximumTender().SetQuantity(fuelMax);

               auto out = ut::log::info() << "Transactor has set container max quantity to platform fuel limit.";
               out.AddNote() << "Transactor: " << MyFullName();
               out.AddNote() << "Container: " << mContainerPtr->GetName();

               if (mContainerPtr->CurrentQuantity() > fuelMax)
               {
                  mContainerPtr->SetQuantity(fuelMax);
               }
            }
         }
      }
   }

   if (mIsHookedToPayload)
   {
      if (IsService())
      {
         auto out = ut::log::error() << "It is non-sensical to hook a 'service' to payload mass.";
         out.AddNote() << "Transactor: " << MyFullName();
         return false;
      }

      // There is no ability to limit a payload amount in the platform, but we will use the size
      // container set by the end-user to cap the maximum payload we can shove into the platform.
      // Accordingly, what we enforce here is that every time the tranactor changes the mass here,
      // we increment or decrement the platform payload by a corresponding amount.  Since there may
      // be more than one transactor hooking itself to the payload, we cannot guarantee sensical values.
      if (mIsExclusiveHook)
      {
         // We own the payload, so set it according to our container's limits.
         mExchangeProcPtr->GetPlatform()->SetPayloadMass(mContainerPtr->CurrentQuantity());
      }
   }

   bool success =
      mQuery.Initialize(mExchangeProcPtr->GetPlatform(), mIsOfferor, mContainerPtr->MaximumTender().GetNameId(), IsService());

   return success;
}

WsfSimulation* Transactor::GetSimulation() const
{
   return mExchangeProcPtr->GetSimulation();
}

// =========================================================================
bool Transactor::DebugEnabled() const
{
   return mExchangeProcPtr->DebugEnabled();
}

// =========================================================================
bool Transactor::RequestFrom(double aSimTime, const WsfPlatform* aSupplierPtr, const Tender& aTender)
{
   // Before we begin this process, assure that all is ready to begin:
   WsfStringId sId = mQuery.StateId();

   if ((!(sId == sSTATE_READY || sId == sSTATE_REQUESTING)) ||
       (!mContainerPtr->MaximumTender().IsCompatibleWith(aTender)) || (aSupplierPtr == nullptr))
   {
      return false;
   }

   // Assume that if we are making a request, we DO
   // want to be enabled for it... duh.
   SetEnable(true);

   // Direct the request to the platform we think is close by, and
   // we think is offering the type of product or service we need.
   // (NOTE:  This is NOT a guarantee there is an exact match,
   //  and he may not respond with an Offer.)

   mStartTime           = aSimTime;
   size_t       myIndex = mExchangeProcPtr->GetPlatform()->GetIndex();
   unsigned int rid     = Transactor::NextSerialRequestId();
   assert(mQuery.GetResultValue() == Result::cTIMED_OUT);
   mQuery.FormRequest(myIndex,
                      myIndex,
                      aSupplierPtr->GetIndex(),
                      aTender.Quantity(),
                      aTender.Rate(),
                      rid,
                      ContainerPtr()->MaximumTender().GetNameId(),
                      IsOfferor(),
                      IsService());
   mQuery.SetResult(Result::cPENDING);

   if (DebugEnabled())
   {
      std::string supName = aSupplierPtr->GetName();
      auto        out     = ut::log::debug() << "Platform is requesting.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << OwningPlatformName();
      out.AddNote() << "Quantity: " << aTender.Quantity();
      out.AddNote() << "Item: " << aTender.GetNameId();
      out.AddNote() << "Supplier: " << supName;
      out.AddNote() << "Rate: " << aTender.Rate();
      out.AddNote() << "ID: " << rid;
   }

   WsfObserver::ExchangeQueried(mExchangeProcPtr->GetSimulation())(aSimTime, mQuery);

   // Queue timeout cancel in mResponseTimeOutInterval, in case transaction does not succeed.
   bool isTimeOut = true;
   mExchangeProcPtr->GetPlatform()->GetSimulation()->AddEvent(
      ut::make_unique<QueuedEvent>(aSimTime + mTimeOutClockInterval, rid, mQuery.OwningIndex(), this, isTimeOut));

   return true;
}

// =========================================================================
bool Transactor::Offer(double aSimTime, const Query& aQuery)
{
   // Do not change any internal state variables yet, until we assure all is a GO...
   // A number of entry criteria must be correct before a transaction may be pursued.
   // It is more code efficient to chain these all together, but for debugging, they
   // are broken apart.  (Later debugging eventually rendered all these checks moot.)
   /*if (! mIsEnabled) {
      // If we are not enabled yet, completely forget it.
      return false;
   }
   if (mQuery.GetResultValue() != Result::cPENDING) {
      // At the time of Reserve() all result values set to Pending
      return false;
   }
   if (mQuery.RequestId() != 0) {
      // Do we have something else in progress?
      return false;
   }
   if (aQuery.RespondingIndex() != mQuery.OwningIndex()) {
      // This request should be addressed specifically to me.
      return false;
   }
   if (! mContainerPtr->IsCompatibleWith(aQuery.GetTender())) {
      return false;
   }*/

   // When an Offer is made, we will set the negotiated amount (from Zero) to the least
   // of the following:  My Reserve, Container Available, His Request.  This will reduce
   // the amount of our current offer, but can be recovered from mAttenuatedOfferAmount.
   double offeredQuantity =
      MinThatIsNotZero(mQuery.OfferedQuantity(), mContainerPtr->CurrentQuantity(), aQuery.DesiredQuantity());

   double offeredRate = MinThatIsNotZero(mContainerPtr->MaximumTender().Rate(), aQuery.DesiredRate());
   size_t myIndex     = mExchangeProcPtr->GetPlatform()->GetIndex();

   bool ok = mQuery.FormOffer(myIndex,
                              aQuery.OwningIndex(),
                              myIndex,
                              aQuery.DesiredQuantity(),
                              aQuery.DesiredRate(),
                              offeredQuantity,
                              offeredRate,
                              aQuery.RequestId(),
                              ContainerPtr()->MaximumTender().GetNameId(),
                              IsOfferor(),
                              IsService());
   ok &= (offeredQuantity > 0.0);

   if (ok)
   {
      // All checks have passed in order to begin a Transaction:
      mStartTime = aSimTime;

      mQuery.SetResult(Result::cPENDING);

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Platform is offering.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << OwningPlatformName();

         auto dump = out.AddNote() << "Debug Dump:";
         DebugDump(aSimTime, mQuery, dump);
      }

      WsfObserver::ExchangeQueried(mExchangeProcPtr->GetPlatform()->GetSimulation())(aSimTime, mQuery);

      // Queue timeout cancel in mResponseTimeOutInterval, in case transaction does not succeed.
      bool isTimeOut = true;
      mExchangeProcPtr->GetPlatform()->GetSimulation()->AddEvent(
         ut::make_unique<QueuedEvent>(aSimTime + mTimeOutClockInterval, aQuery.RequestId(), mQuery.OwningIndex(), this, isTimeOut));
   }

   return ok;
}

// =========================================================================
bool Transactor::Accept(double aSimTime, const Query& aQuery)
{
   mQuery.SetState(sSTATE_ACCEPTING);
   mQuery.SetEvent(sEVENT_ACCEPTED);
   mQuery.SetOfferedQuantity(aQuery.OfferedQuantity());
   mQuery.SetOfferedRate(aQuery.OfferedRate());

   // The negotiated quantity, which was previously zero,
   // will be set to the least of my Request, and his negotiated amount:
   double negotiatedAmount = mQuery.DesiredQuantity();
   double hisLimit         = aQuery.NegotiatedQuantity();
   if (hisLimit < negotiatedAmount)
   {
      negotiatedAmount = hisLimit;
   }
   mQuery.SetNegotiatedQuantity(negotiatedAmount);

   // The negotiated rate, which was previously zero,
   // will be set to the least of my Rate, and his negotiated rate:
   mQuery.SetNegotiatedRate(MinThatIsNotZero(mQuery.DesiredRate(), aQuery.OfferedRate(), aQuery.NegotiatedRate()));

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform is accepting.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << OwningPlatformName();
      out.AddNote() << "Quantity: " << mQuery.NegotiatedQuantity();
      out.AddNote() << "Item: " << mQuery.ItemId();
      out.AddNote() << "Rate: " << mQuery.NegotiatedRate();
      out.AddNote() << "ID: " << mQuery.RequestId();
   }

   WsfObserver::ExchangeQueried(mExchangeProcPtr->GetPlatform()->GetSimulation())(aSimTime, mQuery);

   return NegotiationComplete(aSimTime, aQuery.NegotiatedQuantity());
}

// =========================================================================
bool Transactor::Supply(double aSimTime)
{
   // Notice that this function uses only "My" Query,
   // but none from "His" Query.  The reason for this is
   // that the order of ExchangeQueried callbacks may mean
   // that "His" transaction is complete before mine, and
   // he will have already been set back to zero.

   if ((!mIsEnabled) || (mQuery.StateId() != sSTATE_OFFERING))
   {
      // This Supply makes no sense.  Ignore it.
      return false;
   }

   mQuery.SetState(sSTATE_SUPPLYING);
   mQuery.SetEvent(sEVENT_SUPPLIED);

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform is supplying.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << OwningPlatformName();
      out.AddNote() << "Quantity: " << mQuery.OfferedQuantity();
      out.AddNote() << "Item: " << mQuery.ItemId();
      out.AddNote() << "Rate: " << mQuery.NegotiatedRate();
      out.AddNote() << "ID: " << mQuery.RequestId();
   }

   // This will generate the callback to DIS, if required...
   WsfObserver::ExchangeQueried(mExchangeProcPtr->GetPlatform()->GetSimulation())(aSimTime, mQuery);

   return NegotiationComplete(aSimTime, mQuery.NegotiatedQuantity());
}

// =========================================================================
bool Transactor::RepairWasCompleted(double aSimTime)
{
   assert(IsRequester() && IsService() && (mQuery.GetResultValue() == Result::cIN_PROGRESS));

   mQuery.SetEvent(sEVENT_REQUESTOR_RESPONDED);
   mQuery.SetState(sSTATE_READY);

   // This should cause a confirmation PDU to be sent, if our repairer was external (?).
   WsfObserver::ExchangeQueried(mExchangeProcPtr->GetSimulation())(aSimTime, mQuery);

   // What follows is a streamlined version of ConsummateNow, but knowing it is
   // for services only, and cannot be linked to payload or fuel...
   double repairAmount = mQuery.NegotiatedQuantity();
   ContainerPtr()->Increment(repairAmount);
   DetermineFinalResult();

   // ==========================================================================================
   WsfObserver::ExchangeCompleted(mExchangeProcPtr->GetPlatform()->GetSimulation())(aSimTime, *this, mQuery);
   // ==========================================================================================

   mIsEnabled = false;
   mQuery.FinalReset();

   return true;
}

// =========================================================================
bool Transactor::NegotiationComplete(double aSimTime, double aNegotiatedQuantity)
{
   // We have now committed to a future transaction.
   // Inform the rest of the simulation.
   if (mQuery.GetResultValue() == Result::cIN_PROGRESS)
   {
      // Apparently we have already done this.  Ignore.
      return true;
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Platform has completed negotiation.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << OwningPlatformName();
      out.AddNote() << "Quantity: " << mQuery.NegotiatedQuantity();
      out.AddNote() << "Item: " << mQuery.ItemId();
      out.AddNote() << "Rate: " << mQuery.NegotiatedRate();
      out.AddNote() << "ID: " << mQuery.RequestId();
   }

   mQuery.SetResult(Result::cIN_PROGRESS);

   // This indicates that we have committed to supplying this stuff.
   WsfObserver::ExchangeNegotiated(mExchangeProcPtr->GetPlatform()->GetSimulation())(aSimTime, *this, mQuery);

   if (mIsOfferor)
   {
      // Take the amount negotiated from the container, as we have
      // committed it to the transaction.  (If the transaction fails before
      // completion, later we put this amount back into the container.)
      mReservedFromContainer = mContainerPtr->Decrement(aNegotiatedQuantity);
      mQuery.SetNegotiatedQuantity(mReservedFromContainer);
   }

   if (IsRequester() && IsService())
   {
      // Special Treatment to the 'needs_repair' platform that has just negotiated
      // a repair completed by another platform...  Only the repairer can tell us
      // when we are repaired.  The length of time taken for that repair may or
      // may not match the forecast.  We just wait for his response...
      return true;
   }

   // So the amount and rate have been negotiated.
   // This implies a transaction interval...
   // Queue the transaction to end when the interval elapses:
   double interval = mQuery.NegotiatedInterval();
   if (interval == 0.0 || mExchangeProcPtr->ForceTransactionsInstantaneous())
   {
      // Complete the transaction without further delay:
      return ConsummateNow(aSimTime);
   }
   else
   {
      // Queue a Consummate in the future, if not Cancelled early...
      bool isTimeOut = false; // else ConsummateNow()
      mExchangeProcPtr->GetPlatform()->GetSimulation()->AddEvent(
         ut::make_unique<QueuedEvent>(aSimTime + interval, mQuery.RequestId(), mQuery.OwningIndex(), this, isTimeOut));
   }
   return true;
}

// =========================================================================
bool Transactor::ConsummateNow(double aSimTime)
{
   if (mQuery.StateId() == sSTATE_READY)
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Platform received CONSUMMATE_NOW command, but was READY, so ignored.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << OwningPlatformName();
      }
      return false;
   }

   bool timedOut;
   if (mQuery.GetResultValue() == Result::cPENDING)
   {
      // This is only a time-out.  If so, nothing should be transacted:
      mQuery.SetNegotiatedQuantity(0.0);
      timedOut = true;
   }
   else
   {
      timedOut = false;
   }

   // This Transaction could have been canceled early.  Check for this:
   double ratio = 1.0;
   if (!mExchangeProcPtr->ForceTransactionsInstantaneous())
   {
      double denom = mQuery.NegotiatedInterval();
      if (denom > 0.0)
      {
         ratio = (aSimTime - mStartTime) / denom;
         if (ratio > 1.0)
         {
            ratio = 1.0;
         }
      }
   }
   double transactedQuantity = ratio * mQuery.NegotiatedQuantity();

   if (IsService() && IsOfferor())
   {
      // If IsService() and IsOfferor(), then tell the repaired platform he is ready to go...
      mQuery.SetEvent(sEVENT_OFFEROR_COMPLETED);
      WsfObserver::ExchangeQueried(mExchangeProcPtr->GetSimulation())(aSimTime, mQuery);
   }

   if (transactedQuantity > 0.0)
   {
      // Manage the contents of our container:
      // (If an Offeror, this quantity should have already been reserved
      //  from the container, so we do NOT take it out now !!!)
      if (mIsOfferor)
      {
         // We already removed a quantity from the Container.
         // Assure/Confirm we were right with the amount.

         if (DebugEnabled())
         {
            double newQty = mContainerPtr->CurrentQuantity();
            auto   out    = ut::log::debug() << "Offerer has lost quantity.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << OwningPlatformName();
            out.AddNote() << "Transactor: " << GetName();
            out.AddNote() << "Container: " << mContainerPtr->GetName();
            out.AddNote() << "Quantity Lost: " << transactedQuantity;
            out.AddNote() << "Previous Quantity: " << newQty + transactedQuantity;
            out.AddNote() << "New Quantity: " << newQty;
         }

         double delta = mReservedFromContainer - transactedQuantity;
         assert(fabs(delta) < cNEARLY_ZERO);
         if (delta > cNEARLY_ZERO)
         {
            // We took too much from the Container Earlier.
            // Quietly put the excess back in, without fanfare...
            /*double incrementedQuantity =*/
            mContainerPtr->Increment(delta);
            /*assert(incrementedQuantity == delta);*/
         }
      }
      else
      {
         if (DebugEnabled())
         {
            double prevQty = mContainerPtr->CurrentQuantity();
            auto   out     = ut::log::debug() << "Receiver is adding quantity.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << OwningPlatformName();
            out.AddNote() << "Transactor: " << GetName();
            out.AddNote() << "Container: " << mContainerPtr->GetName();
            out.AddNote() << "Quantity Gained: " << transactedQuantity;
            out.AddNote() << "Previous Quantity: " << prevQty;
            out.AddNote() << "New Quantity: " << prevQty + transactedQuantity;
         }
         assert(mReservedFromContainer == 0.0);
         /*double incrementedQuantity =*/
         mContainerPtr->Increment(transactedQuantity);
         /*assert(incrementedQuantity == transactedQuantity);*/
      }

      // Now manage the contents of the Platform:
      WsfPlatform* platPtr = mExchangeProcPtr->GetPlatform();
      if (platPtr != nullptr)
      {
         if (mIsHookedToFuel)
         {
            WsfFuel* fuelPtr = mExchangeProcPtr->GetPlatform()->GetComponent<WsfFuel>();

            if (mIsExclusiveHook)
            {
               // This flag means that we and we alone control/set the
               // entire PLATFORM fuel quantity.  So whatever is in our
               // Container, we set the same identical amount in our Fuel object.
               // Container has already been Incremented/Decremented above,
               // so just Set it into the quantity.
               double containerQuantity = mContainerPtr->CurrentQuantity();
               if (fuelPtr == nullptr)
               {
                  platPtr->SetFuelMass(containerQuantity);
               }
               else
               {
                  fuelPtr->SetQuantityRemaining(containerQuantity);
               }
            }
            else
            {
               // Here we do not control fuel quantities directly,
               // we only increment or decrement them.
               double beginningMass;
               if (fuelPtr == nullptr)
               {
                  beginningMass = platPtr->GetFuelMass();
               }
               else
               {
                  beginningMass = fuelPtr->GetQuantityRemaining();
               }

               double signedTransacted = (IsOfferor()) ? -transactedQuantity : transactedQuantity;
               double endingMass       = beginningMass;
               endingMass += signedTransacted;

               if (fuelPtr == nullptr)
               {
                  platPtr->SetFuelMass(endingMass);
               }
               else
               {
                  fuelPtr->SetQuantityRemaining(endingMass);
               }
            }
         }
         else if (mIsHookedToPayload)
         {
            if (IsExclusiveHook())
            {
               // This flag means that we and we alone control/set the
               // entire PLATFORM payload quantity.  So whatever is in our
               // Container, we set the same identical amount in our Payload.
               // Container has already been Incremented/Decremented above,
               // so just Set it into the quantity.
               double containerQuantity = mContainerPtr->CurrentQuantity();
               platPtr->SetPayloadMass(containerQuantity);
            }
            else
            {
               // Here we do not control payload quantities directly,
               // we only increment or decrement them.
               double beginningMass = platPtr->GetPayloadMass();
               double endingMass    = beginningMass;
               if (mIsOfferor)
               {
                  endingMass -= transactedQuantity;
               }
               else
               {
                  endingMass += transactedQuantity;
               }

               if (endingMass < 0.0)
               {
                  endingMass = 0.0;
               }
               platPtr->SetPayloadMass(endingMass);
            }
         }
      }
   }
   else
   {
      UnreserveFromContainer();
   }
   mReservedFromContainer = 0.0;

   DetermineFinalResult(timedOut);

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "CONSUMMATE_NOW processed.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << OwningPlatformName();
      out.AddNote() << "Quantity: " << transactedQuantity;
      out.AddNote() << "Item: " << mQuery.ItemId();
      out.AddNote() << "Rate: " << mQuery.NegotiatedRate();
      out.AddNote() << "ID: " << mQuery.RequestId();
      out.AddNote() << "Calling back to observers.";
   }

   // ==========================================================================================
   // This indicates that we have committed to supplying this stuff.
   if (timedOut)
   {
      WsfObserver::ExchangeRequestFailed(mExchangeProcPtr->GetPlatform()->GetSimulation())(aSimTime, *this, mQuery);
   }
   else
   {
      WsfObserver::ExchangeCompleted(mExchangeProcPtr->GetPlatform()->GetSimulation())(aSimTime, *this, mQuery);
   }
   // ==========================================================================================

   if (DebugEnabled())
   {
      auto out = ut::log::debug()
                 << "Post Completion Callback: CONSUMMATE_NOW is resetting the transactor back to sSTATE_READY.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << OwningPlatformName();
      out.AddNote() << "Transactor: " << GetName();
   }

   bool isTotallyDone = true;

   if (mIsOfferor)
   {
      if (mIsContinueToOffer)
      {
         mAttenuatedOfferAmount -= mQuery.NegotiatedQuantity();
         if (mAttenuatedOfferAmount < 0.0)
         {
            mAttenuatedOfferAmount = 0.0;
         }
      }
      else
      {
         mAttenuatedOfferAmount = 0.0;
      }

      if (mAttenuatedOfferAmount > 0.0)
      {
         isTotallyDone = false;
         mQuery.ContinuingReset(mAttenuatedOfferAmount);
      }
   }

   if (isTotallyDone)
   {
      mIsEnabled = false;
      mQuery.FinalReset();

      if (IsOfferor())
      {
         // We are done, so withdraw our Capability offering.
         assert(!mQuery.ItemId().IsNull());
         CapabilityType capability = mExchangeProcPtr->GetCapabilityOf(mQuery.ItemId());
         SetCapabilityFlag(mExchangeProcPtr->GetPlatform(), capability, false);
      }
   }

   return true;
}


// =========================================================================
void Transactor::UnreserveFromContainer()
{
   // Previously we took something from the container,
   // but did not use it.  Put it back.
   if (mReservedFromContainer > 0.0)
   {
      /*double incrementedQuantity =*/mContainerPtr->Increment(mReservedFromContainer);
      /*assert(incrementedQuantity == mReservedFromContainer);*/
      mReservedFromContainer = 0.0;
   }
}

// =========================================================================
void Transactor::DetermineFinalResult(bool aWasTimedOut)
{
   Result::Value theResult = Result::cPARTLY_SATISFIED;
   if (aWasTimedOut)
   {
      theResult = Result::cTIMED_OUT;
   }
   else
   {
      // Determine the final Result status which all will now see when the
      // COMPLETED callback hits their in box.  The result is subjective.
      // A transaction is fully successful from the Supplier's perspective
      // if he gave all he offered.  From the receiver's perspective, a
      // success is receiving all he wanted.  Partially successful if some
      // positive quantity was exchanged, and rejected otherwise.

      if (mIsOfferor)
      {
         if (fabs(mQuery.OfferedQuantity() - mQuery.NegotiatedQuantity()) < cNEARLY_ZERO)
         {
            theResult = Result::cFULLY_SATISFIED;
         }
         else if (fabs(mQuery.NegotiatedQuantity()) < cNEARLY_ZERO)
         {
            theResult = Result::cCANCELLED_REJECTED;
         }
      }
      else
      {
         if (fabs(mQuery.DesiredQuantity() - mQuery.NegotiatedQuantity()) < cNEARLY_ZERO)
         {
            theResult = Result::cFULLY_SATISFIED;
         }
         else if (fabs(mQuery.NegotiatedQuantity()) < cNEARLY_ZERO)
         {
            theResult = Result::cCANCELLED_REJECTED;
         }
      }
   }
   mQuery.SetResult(theResult);
}

// =========================================================================
void Transactor::Cancel(double aSimTime)
{
   // More than likely, this is a queued response to a timeout event.
   ConsummateNow(aSimTime);
}

// =========================================================================
// virtual
void Transactor::TakeQueuedAction(double aSimTime, unsigned int aRequestId, bool aIsTimeOut)
{
   // Ignore this event as OBE (overcome by events) if RID does not match:
   if (aRequestId != mQuery.RequestId())
   {
      return;
   }

   if (aIsTimeOut)
   {
      if (!mQuery.IsDone())
      {
         // But wait, a further check is required...  This is a TIMED Cancel.
         // If the negotiation became complete during the intervening time,
         // ignore the timeout.
         if (mQuery.GetResultValue() != Result::cIN_PROGRESS)
         {
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Transactor's queued action CANCEL called.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << OwningPlatformName();
               out.AddNote() << "Transactor: " << GetName();
            }
            Cancel(aSimTime);
         }
      }
      else
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug()
                       << "Transactor's queued action CANCEL was null, as requestId did not match or state was READY.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << OwningPlatformName();
            out.AddNote() << "Transactor: " << GetName();
         }
      }
   }
   else // IsConsummateNow()
   {
      ConsummateNow(aSimTime);
   }
}

// =========================================================================
bool Transactor::ExchangeQueried(double aSimTime, const Query& aQuery)
{
   // ================================================================
   // THERE IS A MULTI-LEVEL CHECK PROCESS TO ENABLE A TRANSACTION:
   // 1.  Quick-And-Dirty top level filter (A and B).
   // 2.  Addressee second level check.
   // 3.  StateMachine calculation.
   // 4.  If and only if the above checks pass, then we have to
   //        consider specific details to see if the transaction
   //        can be accomplished.
   // ================================================================

   if (!mIsEnabled)
   {
      // 1. A. - Never respond to any Query when not enabled.
      return false;
   }

   if (&mQuery == &aQuery)
   {
      // 1. B. - Never respond to any Query that is "myself".
      return false;
   }

   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Transactor Debug Dump:";
      DebugDump(aSimTime, aQuery, out);
   }

   // 1.  LEVEL ONE Quick-And-Dirty checks:
   bool isIdMismatch = (!mQuery.ItemId().IsNull()) && (!aQuery.ItemId().IsNull()) && (mQuery.ItemId() != aQuery.ItemId());

   if ((mQuery.IsDone()) || (aQuery.IsDone()) || /// hmmm, may need to remove this later?
       (mQuery.IsService() != aQuery.IsService()) || (mQuery.IsOfferor() == aQuery.IsOfferor()) ||
       (mQuery.OwningIndex() == aQuery.OwningIndex()) || // no intra-platform transfers
       (isIdMismatch))
   {
      // Level 1. Reject...
      if (DebugEnabled())
      {
         ut::log::debug() << "<RESULT = NO-OP>";
      }
      return false;
   }

   // 2.  Check to assure that I either have a supply or a need, or he does.
   // (Because if my quantity is currently Zero, or his, how can we possibly transact?)
   if (((mQuery.OfferedQuantity() == 0.0) && (mQuery.DesiredQuantity() == 0.0)) ||
       ((aQuery.OfferedQuantity() == 0.0) && (aQuery.DesiredQuantity() == 0.0)))
   {
      // Level 2 Reject...
      // This is an odd occurrence.  We maybe should annotate it.
      // ut::log::warning() << "A transaction was discounted due to supplier/receiver quantity null.";
      if (DebugEnabled())
      {
         ut::log::debug() << "<RESULT = NO-OP>";
      }
      return false;
   }

   // 3.  Addressee check:  A request or offer _MUST_ be specifically addressed to me, no one else.
   if (!((aQuery.RequestingIndex() == mQuery.OwningIndex()) || (aQuery.RespondingIndex() == mQuery.OwningIndex())))
   {
      // Level 3 Reject...
      if (DebugEnabled())
      {
         ut::log::debug() << "<RESULT = NO-OP>";
      }
      return false;
   }

   // Here we only examine what the possible new state would be...
   // We cannot yet set that state internally, as other issues may
   // preclude us from transitioning.
   WsfStringId newState     = mQuery.StateMachine(aQuery.EventId());
   bool        stateChanged = newState != mQuery.StateId();
   if (stateChanged)
   {
      if (newState == mQuery.StateId())
      {
         // State machine says that no transition is needed...  Ignore.
         if (DebugEnabled())
         {
            ut::log::debug() << "<RESULT = NO-OP>";
         }
         return false;
      }
      else if (newState == sSTATE_OFFERING)
      {
         // It appears that all is in readiness to Offer goods or services.
         // But first we must pass a (possible) proximity check.
         if (IsInProximityTo(aQuery.RequestingIndex()))
         {
            if (DebugEnabled())
            {
               ut::log::debug() << "<RESULT = Possible OFFER>";
            }
            return Offer(aSimTime, aQuery); // *** *** *** *** *** *** *** *** *** *** ***
         }
         else if (DebugEnabled())
         {
            ut::log::debug() << "Note that no Offer() was not made, purely due to lack of proximity.";
         }
         if (DebugEnabled())
         {
            ut::log::debug() << "<RESULT = NO-OP>";
         }
         return false;
      }
      else if (newState == sSTATE_ACCEPTING)
      {
         // It appears that all is in readiness to Accept goods or services.
         // But first we must pass a (possible) proximity check.
         if (IsInProximityTo(aQuery.RespondingIndex()))
         {
            if (DebugEnabled())
            {
               ut::log::debug() << "<RESULT = Possible ACCEPT>";
            }
            return Accept(aSimTime, aQuery); // *** *** *** *** *** *** *** *** *** *** ***
         }
         if (DebugEnabled())
         {
            ut::log::debug() << "<RESULT = NO-OP (Proximity Check Failed) >";
         }
         return false;
      }
      else if (newState == sSTATE_SUPPLYING)
      {
         if (mQuery.RequestId() == aQuery.RequestId())
         {
            if (DebugEnabled())
            {
               ut::log::debug() << "<RESULT = Possible SUPPLY>";
            }
            return Supply(aSimTime); // *** *** *** *** *** *** *** *** *** *** ***
         }
         if (DebugEnabled())
         {
            ut::log::debug() << "<RESULT = NO-OP (RequestIds Mismatched)>";
         }
         return false;
      }
      else if (newState == sSTATE_READY)
      {
         // special case for the repaired entity that has just been told the reapir is complete:
         if (aQuery.EventId() == sEVENT_OFFEROR_COMPLETED)
         {
            if (DebugEnabled())
            {
               ut::log::debug() << "<RESULT = REPAIR RESPONSE>";
            }
            return RepairWasCompleted(aSimTime); // *** *** *** *** *** *** *** *** *** *** ***
         }
         else
         {
            if (DebugEnabled())
            {
               ut::log::debug() << "<RESULT = NEGOTIATION COMPLETE>";
            }
            return NegotiationComplete(aSimTime, aQuery.NegotiatedQuantity()); // *** *** *** *** *** *** *** *** *** *** ***
         }
      }
      else
      {
         { // RAII block
            auto out = ut::log::fatal() << "Transactor has reached an invalid state.";
            out.AddNote() << "Platform: " << OwningPlatformName();
            out.AddNote() << "Transactor: " << GetName();
            out.AddNote() << "State: " << newState;
         }
         assert(false);
      }
   }
   return false;
}

// =========================================================================
/*bool Transactor::ExchangeNegotiated(double       aSimTime,
                                    const Query& aQuery)
{
   if ((aQuery.OwningIndex() == mExchangeProcPtr->GetPlatform()->GetIndex()) &&
         (mQuery.RequestId() == aQuery.RequestId()))
   {
      if (mIsOfferor)
      {
         // Take the amount negotiated from the container, as we have
         // committed it to the transaction.  (If the transaction fails
         // before completion, we put this amount back into the container.
         mReservedFromContainer = mContainerPtr->Decrement(aQuery.NegotiatedQuantity());
         mQuery.SetNegotiatedQuantity(mReservedFromContainer);
      }
      return true;
   }
   return false;
}*/

// =========================================================================

// ============= Begin Transactor Sub-Class QueuedEvent  ===================
Transactor::QueuedEvent::QueuedEvent(double       aSimTime,
                                     unsigned int aRequestId,
                                     size_t       aPlatformIndex,
                                     Transactor*  aTransactorPtr,
                                     bool         aIsTimeOut)
   : WsfEvent(aSimTime)
   , mIsTimeOut(aIsTimeOut)
   , mRequestId(aRequestId)
   , mPlatIndex(aPlatformIndex)
   , mTransactorPtr(aTransactorPtr)
{
}

// =========================================================================
// virtual
WsfEvent::EventDisposition Transactor::QueuedEvent::Execute()
{
   if (GetSimulation()->PlatformExists(mPlatIndex))
   {
      mTransactorPtr->TakeQueuedAction(GetTime(), mRequestId, mIsTimeOut);
   }
   return cDELETE;
}
// ============= End Transactor Sub-Class QueuedEvent  ===================

} // end namespace WsfExchange
