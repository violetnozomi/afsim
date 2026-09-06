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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "WsfTankedFuel.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfFuelObserver.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"

// static
WsfStringId WsfTankedFuel::TypeId()
{
   return "WSF_TANKED_FUEL";
}

WsfTankedFuel::WsfTankedFuel(WsfScenario& aScenario)
   : WsfTabularRateFuel(aScenario)
   , mStations()
   , mReceiveMethod(WsfFuelingOperation::cNO_METHOD)
   , mSupplyMethPref(WsfFuelingOperation::cNO_METHOD)
   , mSupplyLocPref(WsfFuelingOperation::cNO_PREFERENCE)
   , mReceiveInProcess(false)
   , mTankInstantly(true)
   , mIsBelowTriggered(true)
   , mIsAboveTriggered(true)
   , mSupplierIndex(0)
   , mSupplyTankPtr(nullptr)
   , mLastFoUpdate(0.0)
   , mMaxReceiveRate(0.0)
   , mCurrentReceiveRate(0.0)
   , mCurrentSupplyRate(0.0)
   , mOperBandMaximum(0.0)
   , mOperBandMinimum(0.0)
{
}


/** Copy constructor (for Clone())
 */
// protected
WsfTankedFuel::WsfTankedFuel(const WsfTankedFuel& aSrc)
   : WsfTabularRateFuel(aSrc)
   , mStations()
   , mReceiveMethod(aSrc.mReceiveMethod)
   , mSupplyMethPref(aSrc.mSupplyMethPref)
   , mSupplyLocPref(aSrc.mSupplyLocPref)
   , mReceiveInProcess(aSrc.mReceiveInProcess)
   , mTankInstantly(aSrc.mTankInstantly)
   , mIsBelowTriggered(aSrc.mIsBelowTriggered)
   , mIsAboveTriggered(aSrc.mIsAboveTriggered)
   , mSupplierIndex(0)
   , mSupplyTankPtr(nullptr)
   , mLastFoUpdate(aSrc.mLastFoUpdate)
   , mMaxReceiveRate(aSrc.mMaxReceiveRate)
   , mCurrentReceiveRate(aSrc.mCurrentReceiveRate)
   , mCurrentSupplyRate(aSrc.mCurrentSupplyRate)
   , mOperBandMaximum(aSrc.mOperBandMaximum)
   , mOperBandMinimum(aSrc.mOperBandMinimum)
{
   if (!aSrc.mStations.empty())
   {
      mStations.clear();
      mStations = aSrc.mStations;
   }
}

// virtual
WsfTankedFuel::~WsfTankedFuel()
{
   for (const auto& supplyPoint : mStations)
   {
      // The fueling operation should already
      // have been deleted, but just in case...
      delete supplyPoint.mOpPtr;
   }
}

// virtual
WsfFuel* WsfTankedFuel::Clone() const
{
   return new WsfTankedFuel(*this);
}

// virtual
bool WsfTankedFuel::Initialize(double aSimTime)
{
   bool ok = WsfTabularRateFuel::Initialize(aSimTime);

   if ((mReceiveMethod != WsfFuelingOperation::cNO_METHOD) && (mMaxReceiveRate <= 0.0))
   {
      auto out = ut::log::error() << "Fuel is allowed to receive, but was not given a max receive rate.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Fuel: " << GetName();
      ok = false;
   }

   if (mOperBandMaximum == 0.0)
   {
      mOperBandMaximum = GetMaximumQuantity();
   }

   if (mOperBandMinimum == 0.0)
   {
      mOperBandMinimum = GetMaximumQuantity();
   }

   if (mOperBandMinimum > mOperBandMaximum)
   {
      mOperBandMinimum = mOperBandMaximum;
   }

   if (GetQuantityRemaining() > mOperBandMinimum)
   {
      mIsBelowTriggered = false;
   }

   if (GetQuantityRemaining() < mOperBandMaximum)
   {
      mIsAboveTriggered = false;
   }

   ok &= SetDefaultSupplyConfig();

   return ok;
}

// virtual
bool WsfTankedFuel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "maximum_refuel_quantity")
   {
      aInput.ReadValueOfType(mOperBandMaximum, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mOperBandMaximum, 0.0);

      double max   = GetMaximumQuantity();
      double bingo = GetBingoQuantity();

      if (((max != 0.0) && (mOperBandMaximum > max)) || ((bingo != 0.0) && (mOperBandMaximum < bingo)))
      {
         throw UtInput::BadValue(aInput, "Specified 'maximum_refuel_quantity' must be between 'maximum_fuel' and 'bingo_fuel'.");
      }

      if ((mOperBandMinimum != 0.0) && (mOperBandMaximum <= mOperBandMinimum))
      {
         throw UtInput::BadValue(aInput, "Specified 'maximum_refuel_quantity' must be greater than 'desired_top_off_quantity'.");
      }
   }
   else if (command == "desired_top_off_quantity")
   {
      aInput.ReadValueOfType(mOperBandMinimum, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mOperBandMinimum, 0.0);

      double max   = GetMaximumQuantity();
      double bingo = GetBingoQuantity();

      if (((max != 0.0) && (mOperBandMinimum > max)) || ((bingo != 0.0) && (mOperBandMinimum < bingo)))
      {
         throw UtInput::BadValue(aInput, "Specified 'desired_top_off_quantity' must be between 'maximum_fuel' and 'bingo_fuel'.");
      }

      if ((mOperBandMaximum != 0.0) && (mOperBandMaximum <= mOperBandMinimum))
      {
         throw UtInput::BadValue(aInput, "Specified 'desired_top_off_quantity' must be less than 'max_top_off_fuel'.");
      }
   }
   else if (command == "receive_method")
   {
      std::string method;
      aInput.ReadValue(method);
      if (method == "boom")
      {
         mReceiveMethod = WsfFuelingOperation::cBOOM;
      }
      else if (method == "hose")
      {
         mReceiveMethod = WsfFuelingOperation::cHOSE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Receive method options are: 'hose' or 'boom'.");
      }
   }
   else if (command == "maximum_receive_rate")
   {
      aInput.ReadValueOfType(mMaxReceiveRate, UtInput::cMASS_TRANSFER);
      aInput.ValueGreaterOrEqual(mMaxReceiveRate, 0.0);
      mTankInstantly = false;
   }
   else if (command == "supply_method_preference")
   {
      std::string method;
      aInput.ReadValue(method);
      if (method == "hose")
      {
         mSupplyMethPref = WsfFuelingOperation::cHOSE;
      }
      else if (method == "boom")
      {
         mSupplyMethPref = WsfFuelingOperation::cBOOM;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown supply method type.");
      }
   }
   else if (command == "supply_location_preference")
   {
      std::string location;
      aInput.ReadValue(location);
      if (location == "wing")
      {
         mSupplyLocPref = WsfFuelingOperation::cCHOOSE_WING;
      }
      else if (location == "center")
      {
         mSupplyLocPref = WsfFuelingOperation::cCHOOSE_CENTER;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown supply location type.");
      }
   }
   else if (command == "supply_point")
   {
      WsfFuelingOperation::StationType station;
      std::string                      locName;
      aInput.ReadValue(locName);
      if (locName == "left")
      {
         station = WsfFuelingOperation::cLEFT_WING;
      }
      else if (locName == "center")
      {
         station = WsfFuelingOperation::cCENTERLINE;
      }
      else if (locName == "right")
      {
         station = WsfFuelingOperation::cRIGHT_WING;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown refueling supply station name.");
      }

      WsfFuelingOperation::MethodType method;
      std::string                     methName;
      aInput.ReadValue(methName);
      if (methName == "boom")
      {
         method = WsfFuelingOperation::cBOOM;
      }
      else if (methName == "hose")
      {
         method = WsfFuelingOperation::cHOSE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown refueling supply station name.");
      }

      double rate;
      aInput.ReadValueOfType(rate, UtInput::cMASS_TRANSFER);
      aInput.ValueGreaterOrEqual(rate, 0.0);

      mStations.emplace_back(method, station, rate);
   }
   else
   {
      myCommand = WsfTabularRateFuel::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
double WsfTankedFuel::UpdateFuelingOps(double aSimTime)
{
   double offLoad = 0.0;

   if (aSimTime > mLastFoUpdate)
   {
      for (auto& supplyPoint : mStations)
      {
         if (supplyPoint.mOpPtr != nullptr)
         {
            // lowOnFuel must be re-evaluated after each decrement from the tank!
            bool disconnected = IsOnReserve() || IsBingo() || IsEmpty();

            offLoad += supplyPoint.mOpPtr->Update(aSimTime, disconnected);

            // The fueling operation may have terminated itself
            if (disconnected)
            {
               delete supplyPoint.mOpPtr;
               supplyPoint.mOpPtr = nullptr;
            }
         }
      }
      mLastFoUpdate = aSimTime;
   }

   return offLoad;
}

// virtual
void WsfTankedFuel::Update(double aSimTime)
{
   if (aSimTime > GetLastUpdateTime())
   {
      // The mIsAboveTriggered flag is controlled within PushFuelIn(),
      // and the mIsBelowTriggered flag is controlled within PullFuelOut()

      // First thing to do is offload fuel
      // for any tanking operations in progress.
      double offLoad = UpdateFuelingOps(aSimTime);

      mCurrentSupplyRate = offLoad / (aSimTime - GetLastUpdateTime());

      // mLastUpdateTime is refreshed by the base class...
      WsfTabularRateFuel::Update(aSimTime);
   }
}

bool WsfTankedFuel::SetDefaultSupplyConfig()
{
   if (mStations.empty())
   {
      // Doesn't matter what the flag settings are, they won't be used.
      return true;
   }

   if ((mSupplyLocPref != WsfFuelingOperation::cNO_PREFERENCE) && (mSupplyMethPref != WsfFuelingOperation::cNO_METHOD))
   {
      // Preferences were set via input file... (but also may change during run time)
      return true;
   }

   bool centerExists   = false;
   bool boomExists     = false;
   bool wingExists     = false;
   bool hoseExists     = false;
   bool wingBoomExists = false;
   bool wingHoseExists = false;

   bool pairOfWingBooms = false;
   bool pairOfWingHoses = false;

   for (const auto& supplyPoint : mStations)
   {
      bool wingTip = supplyPoint.mStation == WsfFuelingOperation::cLEFT_WING ||
                     supplyPoint.mStation == WsfFuelingOperation::cRIGHT_WING;

      boomExists |= (supplyPoint.mMethod == WsfFuelingOperation::cBOOM);
      hoseExists |= (supplyPoint.mMethod == WsfFuelingOperation::cHOSE);
      centerExists |= (supplyPoint.mStation == WsfFuelingOperation::cCENTERLINE);
      wingExists |= wingTip;

      bool isWingBoom = wingTip && (supplyPoint.mMethod == WsfFuelingOperation::cBOOM);
      if (isWingBoom && wingBoomExists)
      {
         pairOfWingBooms = true;
      }
      wingBoomExists |= isWingBoom;

      bool isWingHose = wingTip && (supplyPoint.mMethod == WsfFuelingOperation::cHOSE);
      if (isWingHose && wingHoseExists)
      {
         pairOfWingHoses = true;
      }
      wingHoseExists |= isWingHose;
   }

   // As an advanced concept experimental study, prefer two wingtip booms, if possible...
   // (I know, no currently existing aircraft has them, but if some conceptual aircraft
   //  has been given them, then it is obvious the preference would be to use them.)
   if (pairOfWingBooms)
   {
      mSupplyMethPref = WsfFuelingOperation::cBOOM;
      mSupplyLocPref  = WsfFuelingOperation::cCHOOSE_WING;
      return true;
   }

   // As a typical USN default, use two wingtip fueling hoses, if possible...
   if ((!boomExists) && (pairOfWingHoses))
   {
      mSupplyMethPref = WsfFuelingOperation::cHOSE;
      mSupplyLocPref  = WsfFuelingOperation::cCHOOSE_WING;
      return true;
   }

   for (const auto& supplyPoint : mStations)
   {
      // As a non-optimal (but more typical) USN use, use one centerline hose, if possible...
      if ((!boomExists) && (supplyPoint.mMethod == WsfFuelingOperation::cHOSE) &&
          (supplyPoint.mStation == WsfFuelingOperation::cCENTERLINE))
      {
         mSupplyMethPref = WsfFuelingOperation::cHOSE;
         mSupplyLocPref  = WsfFuelingOperation::cCHOOSE_CENTER;
         return true;
      }
   }

   for (const auto& supplyPoint : mStations)
   {
      // As a typical USAF default, prefer centerline boom fueling, if possible...
      if ((!hoseExists) && (supplyPoint.mMethod == WsfFuelingOperation::cBOOM) &&
          (supplyPoint.mStation == WsfFuelingOperation::cCENTERLINE))
      {
         mSupplyMethPref = WsfFuelingOperation::cBOOM;
         mSupplyLocPref  = WsfFuelingOperation::cCHOOSE_CENTER;
         return true;
      }
   }
   auto out = ut::log::warning() << "Refueling Supply configuration ambiguous.";
   out.AddNote() << "Platform: " << GetPlatform()->GetName();
   out.AddNote() << "Fuel: " << GetName();
   out.AddNote() << "Communications during run-time must properly configure the tanker for supply operations.";
   return true;
}

// ------------------------------------------------------
// Methods below relate to refueling operations / tanking
// ------------------------------------------------------

// virtual
bool WsfTankedFuel::BeginSupplyingFuelTo(double aSimTime, WsfTankedFuel* aReceiverPtr)
{
   // A client needs to be equipped to receive in-air fueling...
   if (!aReceiverPtr->IsReceiveCapable())
   {
      if (DebugEnabled())
      {
         auto out = ut::log::error() << "Tanker cannot supply fuel to receiver.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Tanker: " << GetPlatform()->GetName();
         out.AddNote() << "Receiver: " << aReceiverPtr->GetPlatform()->GetName();
         out.AddNote() << "Receiver has not enabled his fueling port.";
      }
      return false;
   }

   // A client can only receive fuel from one tanker at a time...
   if (aReceiverPtr->ReceiveInProcess())
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Tanker cannot supply fuel to receiver.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Tanker:" << GetPlatform()->GetName();
         out.AddNote() << "Receiver: " << aReceiverPtr->GetPlatform()->GetName();
         out.AddNote() << "Receiver is already tanking from another.";
      }
      return false;
   }

   WsfFuelingOperation::SupplyPointIter iter;
   if (!FindStationFor(aReceiverPtr, iter))
   {
      if (DebugEnabled())
      {
         auto out = ut::log::error() << "Tanker cannot supply fuel to receiver.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Tanker:" << GetPlatform()->GetName();
         out.AddNote() << "Receiver: " << aReceiverPtr->GetPlatform()->GetName();
         out.AddNote() << "Tanker has no matching fuel station available.";
      }
      return false;
   }

   // We cannot supply fuel while already in Bingo Fuel...
   if (IsBingo())
   {
      if (DebugEnabled())
      {
         auto out = ut::log::error() << "Tanker cannot supply fuel to receiver.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Tanker:" << GetPlatform()->GetName();
         out.AddNote() << "Receiver: " << aReceiverPtr->GetPlatform()->GetName();
         out.AddNote() << "Receiver is already tanking from another.";
      }
      return false;
   }

   // Above are all the possible failures to begin tanking.
   // We have two possible successes:
   // either an instant tanking, or run-time refueling...
   if (aReceiverPtr->mTankInstantly)
   {
      double totalDesired =
         aReceiverPtr->GetOpBandMaximum() + aReceiverPtr->GetReserveQuantity() - aReceiverPtr->GetQuantityRemaining();

      double actualSupply = DecrementQuantity(aSimTime, totalDesired);

      if (actualSupply < totalDesired)
      {
         // We couldn't get all the fuel we wanted:
         aReceiverPtr->IncrementQuantity(aSimTime, actualSupply);
      }
      else
      {
         aReceiverPtr->IncrementQuantity(aSimTime, actualSupply);
      }

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Tanker instantly supplied fuel to receiver.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Tanker:" << GetPlatform()->GetName();
         out.AddNote() << "Receiver: " << aReceiverPtr->GetPlatform()->GetName();
      }
   }
   else
   {
      // Okay, we can fuel him... construct a new FuelingOperation:
      assert(iter->mOpPtr == nullptr);
      iter->mOpPtr =
         new WsfFuelingOperation(aSimTime, this, aReceiverPtr, iter->mMethod, iter->mStation, iter->mConstRate);

      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Tanker began supplying fuel to receiver.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Tanker:" << GetPlatform()->GetName();
         out.AddNote() << "Receiver: " << aReceiverPtr->GetPlatform()->GetName();

         auto boomNote = out.AddNote();
         if (iter->mMethod == WsfFuelingOperation::cBOOM)
         {
            boomNote << "Boom refueling from the ";
         }
         else
         {
            boomNote << "Hose refueling from the ";
         }

         if (iter->mStation == WsfFuelingOperation::cRIGHT_WING)
         {
            boomNote << "right wing station.";
         }
         else if (iter->mStation == WsfFuelingOperation::cLEFT_WING)
         {
            boomNote << "left wing station.";
         }
         else
         {
            boomNote << "centerline station.";
         }
      }
   }

   return true;
}

bool WsfTankedFuel::SupplyInProcess() const
{
   if (mStations.empty())
   {
      return false;
   }

   for (const auto& supplyPoint : mStations)
   {
      if (supplyPoint.mOpPtr != nullptr)
      {
         return true;
      }
   }
   return false;
}

bool WsfTankedFuel::FindStationFor(WsfTankedFuel* aReceiverPtr, WsfFuelingOperation::SupplyPointIter& aStationIter)
{
   aStationIter = mStations.begin();

   if (mStations.empty())
   {
      return false;
   }

   // These flags are only used if "restrictions" are specified for the ConOps of this mission:
   bool centerInUse = false;
   bool wingInUse   = false;

   if (!WsfFuelingOperation::AllowSimultBoomAndHoseFueling() || !WsfFuelingOperation::AllowSimultWingAndCenterlineFueling())
   {
      bool boomInUse = false;
      bool hoseInUse = false;

      // Set the "Permissions" flags.
      WsfFuelingOperation::SupplyPointConstIter iter = mStations.begin();

      for (; iter != mStations.end(); ++iter)
      {
         if (iter->mOpPtr != nullptr)
         {
            if (!WsfFuelingOperation::AllowSimultBoomAndHoseFueling())
            {
               boomInUse |= (iter->mMethod == WsfFuelingOperation::cBOOM);
               hoseInUse |= (iter->mMethod == WsfFuelingOperation::cHOSE);
            }
            if (!WsfFuelingOperation::AllowSimultWingAndCenterlineFueling())
            {
               centerInUse |= (iter->mStation == WsfFuelingOperation::cCENTERLINE);
               wingInUse |= (iter->mStation == WsfFuelingOperation::cLEFT_WING);
               wingInUse |= (iter->mStation == WsfFuelingOperation::cRIGHT_WING);
            }
         }
      }

      if (!WsfFuelingOperation::AllowSimultBoomAndHoseFueling() &&
          (aReceiverPtr->ReceiveMethod() == WsfFuelingOperation::cBOOM) && (hoseInUse))
      {
         // We have a Hose already in use, and we cannot now use a Boom too.
         return false;
      }

      if (!WsfFuelingOperation::AllowSimultBoomAndHoseFueling() &&
          (aReceiverPtr->ReceiveMethod() == WsfFuelingOperation::cHOSE) && (boomInUse))
      {
         // We have a Boom already in use, and we cannot now use a hose too.
         return false;
      }
   }

   for (; aStationIter != mStations.end(); ++aStationIter)
   {
      // Now find the first station that matches the desired refuel criteria:
      if (aStationIter->mMethod == aReceiverPtr->ReceiveMethod())
      {
         // Correct fueling method is matched... check into the possibility further:
         bool matchesBoom =
            (mSupplyMethPref == WsfFuelingOperation::cBOOM) && (aStationIter->mMethod == WsfFuelingOperation::cBOOM);
         bool matchesHose =
            (mSupplyMethPref == WsfFuelingOperation::cHOSE) && (aStationIter->mMethod == WsfFuelingOperation::cHOSE);

         bool matchesCenter = (mSupplyLocPref == WsfFuelingOperation::cCHOOSE_CENTER) &&
                              (aStationIter->mStation == WsfFuelingOperation::cCENTERLINE);
         bool matchesWing = (mSupplyLocPref == WsfFuelingOperation::cCHOOSE_WING) &&
                            ((aStationIter->mStation == WsfFuelingOperation::cLEFT_WING) ||
                             (aStationIter->mStation == WsfFuelingOperation::cRIGHT_WING));

         bool isMatch = (matchesBoom || matchesHose) && (matchesWing || matchesCenter);

         if ((matchesCenter && centerInUse && (!WsfFuelingOperation::AllowSimultWingAndCenterlineFueling())) ||
             (matchesWing && wingInUse && (!WsfFuelingOperation::AllowSimultWingAndCenterlineFueling())))
         {
            // There is a ConOps conflict, so this option is no good.  Go on to the next possibility.
         }
         else if (isMatch && aStationIter->mOpPtr == nullptr)
         {
            return true;
         }
      }
   }

   return false;
}

// virtual
double WsfTankedFuel::IncrementQuantity(double aSimTime, double aDelta)
{
   assert(aDelta >= 0.0);

   // First check to see if this will push us above Maximum
   if (GetQuantityRemaining() + aDelta > GetMaximumQuantity())
   {
      aDelta = GetMaximumQuantity() - GetQuantityRemaining();
   }

   WsfTabularRateFuel::IncrementQuantity(aDelta);

   if ((!mIsAboveTriggered) && (GetQuantityRemaining() >= mOperBandMaximum))
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Fuel level topped off.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform:" << GetPlatform()->GetName();
      }

      mIsAboveTriggered = true;
      WsfObserver::FuelEvent(GetSimulation())(aSimTime, this, "ABOVE_REFUELING_BAND");

      // We have just been topped off, so reset
      // the mIsBelowTriggered flag for future use
      mIsBelowTriggered = false;
   }

   return aDelta;
}

// virtual
double WsfTankedFuel::DecrementQuantity(double aSimTime, double aDelta)
{
   assert(aDelta >= 0.0);

   // First check to see if this will drop us below Bingo or Reserve
   double remains = GetQuantityRemaining();

   // Prevent drop below zero.
   if (remains - aDelta < 0.0)
   {
      aDelta = remains;
   }

   bool willGoBingo   = (!IsBingo()) && ((remains - aDelta) < GetBingoQuantity());
   bool willGoReserve = (!IsOnReserve()) && ((remains - aDelta) < GetReserveQuantity());

   double partialDelta1 = 0.0;

   if (willGoBingo || willGoReserve)
   {
      if (willGoBingo)
      {
         partialDelta1 = std::max(GetQuantityRemaining() - GetBingoQuantity(), 0.0);
      }
      else
      {
         partialDelta1 = std::max(GetQuantityRemaining() - GetReserveQuantity(), 0.0);
      }

      WsfTabularRateFuel::DecrementQuantity(partialDelta1);

      // Trigger the bingo or reserve...
      CheckEvents(aSimTime);
   }

   double partialDelta2 = aDelta - partialDelta1;

   WsfTabularRateFuel::DecrementQuantity(partialDelta2);

   if ((!mIsBelowTriggered) && (GetQuantityRemaining() <= mOperBandMinimum))
   {
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Fuel dropped below top-off level.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform:" << GetPlatform()->GetName();
      }

      mIsBelowTriggered = true;
      WsfObserver::FuelEvent(GetSimulation())(aSimTime, this, "BELOW_REFUELING_BAND");

      // We have just depleted down to 'need top-off', so reset
      // the mIsAboveTriggered flag for future use
      mIsAboveTriggered = false;
   }

   assert((partialDelta1 >= 0.0) && (partialDelta2 >= 0.0));

   return partialDelta1 + partialDelta2;
}

bool WsfTankedFuel::TerminateAll(double aSimTime)
{
   bool one = DisconnectFromTanker(aSimTime);
   bool two = DisconnectFuelingOps(aSimTime);
   return (one || two);
}

bool WsfTankedFuel::DisconnectFuelingOps(double aSimTime)
{
   bool succeededOnce = false;
   for (const auto& supplyPoint : mStations)
   {
      if (supplyPoint.mOpPtr != nullptr)
      {
         bool disconnect = true;
         // Throw away "double" return value.
         supplyPoint.mOpPtr->Update(aSimTime, disconnect);
         succeededOnce |= disconnect;
         // Do not delete the operation except during a normal Update() call.
      }
   }
   return succeededOnce;
}

bool WsfTankedFuel::DisconnectFromTanker(double aSimTime)
{
   // return "true" if a receive was in progress at the time of call.
   bool disconnected = mReceiveInProcess;
   if (disconnected)
   {
      // MemoryDebug::AssertMemory();
      if (GetSimulation()->PlatformExists(mSupplierIndex))
      {
         mSupplyTankPtr->DisconnectFrom(aSimTime, GetPlatform());
      }
      // MemoryDebug::AssertMemory();
      mReceiveInProcess   = false;
      mCurrentReceiveRate = 0.0;
   }
   Update(aSimTime);
   ResetFlags();
   return disconnected;
}

bool WsfTankedFuel::DisconnectFrom(double aSimTime, WsfPlatform* aPlatformPtr)
{
   auto out = ut::log::info() << "WsfTankedFuel::DisconnectFrom()";
   out.AddNote() << "T = " << aSimTime;
   out.AddNote() << "Platform: " << GetPlatform()->GetName();
   out.AddNote() << "Separating Platform = " << aPlatformPtr->GetName();

   bool disconnected = false;
   if (aPlatformPtr->GetIndex() == mSupplierIndex)
   {
      disconnected = DisconnectFromTanker(aSimTime);
   }
   else
   {
      for (const auto& supplyPoint : mStations)
      {
         if ((supplyPoint.mOpPtr != nullptr) && (supplyPoint.mOpPtr->ReceiveTankPtr()->GetPlatform() == aPlatformPtr))
         {
            disconnected = true;
            // This method returns a double indicating the fuel transfer,
            // but amount is also kept internally, so we ignore it here.
            supplyPoint.mOpPtr->Update(aSimTime, disconnected);

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "Tanker stopped supplying fuel to receiver.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Tanker:" << GetPlatform()->GetName();
               out.AddNote() << "Receiver: " << supplyPoint.mOpPtr->ReceiveTankPtr()->GetPlatform()->GetName();

               auto boomNote = out.AddNote();
               if (supplyPoint.mMethod == WsfFuelingOperation::cBOOM)
               {
                  boomNote << "Boom refueling from the ";
               }
               else
               {
                  boomNote << "Hose refueling from the ";
               }

               if (supplyPoint.mStation == WsfFuelingOperation::cRIGHT_WING)
               {
                  boomNote << "right wing station.";
               }
               else if (supplyPoint.mStation == WsfFuelingOperation::cLEFT_WING)
               {
                  boomNote << "left wing station.";
               }
               else
               {
                  boomNote << "centerline station.";
               }
            }
         }
      }
   }
   return disconnected;
}

// virtual
void WsfTankedFuel::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // Find out if we are Supplying to this platform...
   if (mStations.empty())
   {
      return;
   }

   for (auto& supplyPoint : mStations)
   {
      if (supplyPoint.mOpPtr != nullptr)
      {
         if (supplyPoint.mOpPtr->ReceiveTankPtr()->GetPlatform() == aPlatformPtr)
         {
            double simTime    = GetSimulation()->GetSimTime();
            bool   disconnect = true;
            supplyPoint.mOpPtr->Update(simTime, disconnect);
            delete supplyPoint.mOpPtr;
            supplyPoint.mOpPtr = nullptr;
         }
      }
   }
}

bool WsfTankedFuel::ConnectTanker(double aSimTime, size_t aTankerIndex, WsfTankedFuel* aSourcePtr, double aFuelTransferRate)
{
   if (mReceiveInProcess)
   {
      return false;
   }

   Update(aSimTime);

   mSupplierIndex      = aTankerIndex;
   mSupplyTankPtr      = aSourcePtr;
   mCurrentReceiveRate = aFuelTransferRate;
   mReceiveInProcess   = true;

   return true;
}
