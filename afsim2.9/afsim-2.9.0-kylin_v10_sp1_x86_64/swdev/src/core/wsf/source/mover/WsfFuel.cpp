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

#include "WsfFuel.hpp"

#include <algorithm>
#include <limits>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfFuelObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

// =================================================================================================
WsfFuel::WsfFuel(WsfScenario& aScenario)
   : WsfPlatformPart(aScenario, cCOMPONENT_ROLE<WsfFuel>())
   , mLastUpdate(0.0)
   , mReserveQuantity(0.0)
   , mBingoQuantity(0.0)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfFuel", "FUEL"))
   , mOnEmptyScriptPtr(nullptr)
   , mOnReserveScriptPtr(nullptr)
   , mOnBingoScriptPtr(nullptr)
   , mOnRefuelScriptPtr(nullptr)
   , mConsumeRate(0.0)
   , mMaximumQuantity(std::numeric_limits<double>::max())
   , mInitialQuantity(0.0)
   , mCurrentQuantity(0.0)
   , mBingoEventTriggered(true)
   , mReserveEventTriggered(true)
   , mEmptyEventTriggered(true)
   , mFirstDeltaUpdate(true)
   , mMode(nullptr)
{
   SetName("fuel"); // Must not be changed. Can be referenced by 'internal_link' commands
}

// =================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfFuel::WsfFuel(const WsfFuel& aSrc)
   : WsfPlatformPart(aSrc)
   , mLastUpdate(aSrc.mLastUpdate)
   , mReserveQuantity(aSrc.mReserveQuantity)
   , mBingoQuantity(aSrc.mBingoQuantity)
   , mContextPtr(new WsfScriptContext(*aSrc.mContextPtr))
   , mOnEmptyScriptPtr(nullptr)
   , mOnReserveScriptPtr(nullptr)
   , mOnBingoScriptPtr(nullptr)
   , mOnRefuelScriptPtr(nullptr)
   , mConsumeRate(aSrc.mConsumeRate)
   , mMaximumQuantity(aSrc.mMaximumQuantity)
   , mInitialQuantity(aSrc.mInitialQuantity)
   , mCurrentQuantity(aSrc.mCurrentQuantity)
   , mBingoEventTriggered(aSrc.mBingoEventTriggered)
   , mReserveEventTriggered(aSrc.mReserveEventTriggered)
   , mEmptyEventTriggered(aSrc.mEmptyEventTriggered)
   , mFirstDeltaUpdate(aSrc.mFirstDeltaUpdate)
   , mMode(aSrc.mMode)
{
}

// =================================================================================================
// virtual
WsfFuel::~WsfFuel()
{
   if (GetPlatform() != nullptr)
   {
      GetPlatform()->DetachObserver(this); // Detach the OnPlatformUpdated observer
   }
   delete mContextPtr;
}

// =================================================================================================
//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
// virtual
WsfFuel* WsfFuel::Clone() const
{
   return new WsfFuel(*this);
}

// =================================================================================================
void WsfFuel::SetMode(const std::string& aMode)
{
   // The SetMode(string) method is not virtual,
   // but SetMode(int) is, and may dispatch!
   SetMode(WsfStringId(aMode));
}

// =================================================================================================
// virtual
void WsfFuel::SetMode(WsfStringId aMode)
{
   mMode = aMode;
}

// =================================================================================================
// virtual
std::string WsfFuel::GetMode() const
{
   return mMode.GetString();
}

// =================================================================================================
//! Initialize the fuel object.
//!
//! @param aSimTime [input] The current simulation time.
//! @return 'true' if the object was successfully initialized or 'false'
//! if initialization failed.
// virtual
bool WsfFuel::Initialize(double aSimTime)
{
   bool ok = WsfPlatformPart::Initialize(aSimTime); // Initialize before initialize script context
   GetPlatform()->AttachObserver(this);             // Attach the OnPlatformUpdated observer

   ok &= mContextPtr->Initialize(aSimTime, GetPlatform(), this);

   // Capture the script pointers to avoid constant lookups.
   mOnEmptyScriptPtr   = mContextPtr->FindScript("on_empty");
   mOnReserveScriptPtr = mContextPtr->FindScript("on_reserve");
   mOnBingoScriptPtr   = mContextPtr->FindScript("on_bingo");
   mOnRefuelScriptPtr  = mContextPtr->FindScript("on_refuel");

   // Check the initial value; adjust if it is greater than the max
   if (mInitialQuantity > mMaximumQuantity)
   {
      mInitialQuantity = mMaximumQuantity;
      if (DebugEnabled())
      {
         auto out =
            ut::log::warning()
            << "Initial fuel quantity was greater than maximum fuel quantity. Fuel was constrained to the maximum.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Fuel: " << GetName();
         out.AddNote() << "Initial Fuel Quantity: " << mInitialQuantity;
         out.AddNote() << "Maximum Fuel Quantity: " << mMaximumQuantity;
      }
   }

   // Set the initial fuel quantity
   SetQuantityRemaining(mInitialQuantity);
   if (DebugEnabled() && (mInitialQuantity <= 0.0))
   {
      auto out = ut::log::warning() << "Initial fuel quantity is zero or less.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Fuel: " << GetName();
   }

   ResetFlags();

   // Need to save a last update time
   mLastUpdate = aSimTime;

   return ok;
}

// =================================================================================================
// virtual
bool WsfFuel::Initialize2(double aSimTime)
{
   bool ok = WsfPlatformPart::Initialize2(aSimTime);
   ok &= mContextPtr->Initialize2(aSimTime);
   return ok;
}

// =================================================================================================
void WsfFuel::ResetFlags()
{
   if ((mBingoQuantity > 0.0) && (mCurrentQuantity > mBingoQuantity) && (mBingoEventTriggered))
   {
      mBingoEventTriggered = false;
   }

   if ((mReserveQuantity > 0.0) && (mCurrentQuantity > mReserveQuantity) && (mReserveEventTriggered))
   {
      mReserveEventTriggered = false;
   }

   if ((mCurrentQuantity > 0.0) && (mEmptyEventTriggered))
   {
      mEmptyEventTriggered = false;
   }
}

// =================================================================================================
//! Process the fuel object block.
//!
//! @param aInput [input]  The input stream.
//! @return 'true' if the command was recognized (and processed) or 'false'
//! if the command was not one recognized by this class.
// virtual
bool WsfFuel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "maximum_quantity")
   {
      aInput.ReadValueOfType(mMaximumQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mMaximumQuantity, 0.0);
   }
   else if (command == "initial_quantity")
   {
      aInput.ReadValueOfType(mInitialQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mInitialQuantity, 0.0);
      if ((mMaximumQuantity != 0.0) && (mInitialQuantity > mMaximumQuantity))
      {
         throw UtInput::BadValue(aInput, "Specified 'initial_quantity' must not be greater than 'maximum_quantity'.");
      }
   }
   else if ((command == "reserve_quantity") || (command == "reserve_fuel"))
   {
      aInput.ReadValueOfType(mReserveQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mReserveQuantity, 0.0);
      if ((mMaximumQuantity != 0.0) && (mReserveQuantity > mMaximumQuantity))
      {
         throw UtInput::BadValue(aInput, "Specified 'reserve_quantity' must not be greater than 'maximum_quantity'.");
      }
   }
   else if ((command == "bingo_quantity") || (command == "bingo_fuel"))
   {
      aInput.ReadValueOfType(mBingoQuantity, UtInput::cMASS);
      aInput.ValueGreaterOrEqual(mBingoQuantity, 0.0);
   }
   else if (command == "consumption_rate")
   {
      aInput.ReadValueOfType(mConsumeRate, UtInput::cMASS_TRANSFER);
      aInput.ValueGreaterOrEqual(mConsumeRate, 0.0);
   }
   else if (command == "on_bingo")
   {
      mContextPtr->CompileImplicitScript(aInput, "on_bingo", "void");
   }
   else if (command == "on_empty")
   {
      mContextPtr->CompileImplicitScript(aInput, "on_empty", "void");
   }
   else if (command == "on_refuel")
   {
      mContextPtr->CompileImplicitScript(aInput, "on_refuel", "void");
   }
   else if (command == "on_reserve")
   {
      mContextPtr->CompileImplicitScript(aInput, "on_reserve", "void");
   }
   else if ((command == "mode") || (command == "set_mode"))
   {
      std::string mode;
      aInput.ReadValue(mode);
      mMode = mode;
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfPlatformPart::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
//! Callback from platform when the mover is updated.
void WsfFuel::OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr)
{
   Update(aSimTime);
}

double WsfFuel::GetConsumptionRate(const double aAltitude, const double aSpeed)
{
   return CalcConsumptionRateAltitudeSpeed(aAltitude, aSpeed);
}

double WsfFuel::GetNetDepletionRate()
{
   return CalcConsumptionRate() + GetSupplyRate() - GetReceiveRate();
}

double WsfFuel::GetNetDepletionRate(const double aAltitude, const double aSpeed)
{
   return CalcConsumptionRateAltitudeSpeed(aAltitude, aSpeed) + GetSupplyRate() - GetReceiveRate();
}

// =================================================================================================
//! Updates the fuel data.
//!
//! @param aSimTime [input] The current simulation time.
// virtual
void WsfFuel::Update(double aSimTime)
{
   if (aSimTime > mLastUpdate)
   {
      // Update the fuel rates and quantities.
      DecrementQuantity(CalculateDeltaQuantity(aSimTime));
      CheckEvents(aSimTime);
      mLastUpdate = aSimTime;
   }
}

// =================================================================================================
//! Sets the current fuel quantity.
//!
//! @param aQuantity [input] The fuel quantity.
// virtual
void WsfFuel::SetQuantityRemaining(double aQuantity)
{
   mCurrentQuantity = std::max(std::min(mMaximumQuantity, aQuantity), 0.0);
   GetPlatform()->SetFuelMass(mCurrentQuantity);
}

// =================================================================================================
//! Sets the current bingo fuel quantity.
//!
//! @param aQuantity [input] The bingo fuel quantity.
// virtual
void WsfFuel::SetBingoQuantity(double aQuantity)
{
   mBingoQuantity = aQuantity;
   ResetFlags();
   CheckEvents(GetSimulation()->GetSimTime());
}

// =================================================================================================
//! Sets the current reserve fuel quantity.
//!
//! @param aQuantity [input] The reserve fuel quantity.
// virtual
void WsfFuel::SetReserveQuantity(double aQuantity)
{
   mReserveQuantity = aQuantity;
   ResetFlags();
   CheckEvents(GetSimulation()->GetSimTime());
}

// =================================================================================================
//! Gets the number of seconds of fuel represented at the fuel state for current flight conditions.
//!
//! @param aFuelState [input] The fuel state to check.
//! @return Time in seconds of fuel state
double WsfFuel::TimeAt(FuelState aFuelState)
{
   WsfPlatform* platformPtr = GetPlatform();

   // Get platform speed
   double speed(platformPtr->GetSpeed());

   // Get platform altitude
   double latNotUsed(0.0);
   double lonNotUsed(0.0);
   double altitude(0.0);
   platformPtr->GetLocationLLA(latNotUsed, lonNotUsed, altitude);

   return TimeAt(aFuelState, speed, altitude);
}

// =================================================================================================
//! Gets the number of seconds of fuel represented at the fuel state for input flight parameters.
//!
//! @param aFuelState [input] The fuel state to check.
//! @param aSpeed     [input] The speed to use for calculation
//! @param aAltitude  [input] The altitude to use for calculation
//! @return Time in seconds of bingo fuel
double WsfFuel::TimeAt(FuelState aFuelState, const double aSpeed, const double aAltitude)
{
   double timeAt = std::numeric_limits<double>::max();
   double rate   = GetConsumptionRate(aSpeed, aAltitude);
   if (rate > 0.0)
   {
      double quantity = 0.0;
      switch (aFuelState)
      {
      case ON_BINGO:
         quantity = mBingoQuantity;
         break;
      case ON_RESERVE:
         quantity = mReserveQuantity;
         break;
      case ON_EMPTY:
         break;
      default:
         break;
      }
      timeAt = quantity / rate;
   }
   return timeAt;
}

// =================================================================================================
//! Gets the number of seconds of fuel remaining after flying aRoute.
//!
//! @param aRoute [input] The route to use for calculation
//! @return Time in seconds of total fuel remaining
double WsfFuel::FuelSecondsRemaining(const WsfRoute& aRoute)
{
   double timeRemaining = std::numeric_limits<double>::max();
   double rate          = GetConsumptionRate();
   if (rate > 0.0)
   {
      timeRemaining = (mCurrentQuantity / rate) - aRoute.GetTravelTime();
   }
   return timeRemaining;
}

// =================================================================================================
//! Gets the number of seconds of fuel represented by a given number of pounds of fuel.
//!
//! @param aPounds [input] The fuel quantity in pounds (lbs) to use for calculation
//! @return Time in seconds of how long aPounds fuel will last
double WsfFuel::FuelSeconds(const double aPounds)
{
   WsfPlatform* platformPtr = GetPlatform();

   // Get platform speed
   double speed(platformPtr->GetSpeed());

   // Get platform altitude
   double latNotUsed(0.0);
   double lonNotUsed(0.0);
   double altitude(0.0);
   platformPtr->GetLocationLLA(latNotUsed, lonNotUsed, altitude);

   return FuelSeconds(aPounds, speed, altitude);
}

// =================================================================================================
//! Gets the number of seconds of fuel represented by a given number of pounds of fuel.
//!
//! @param aPounds   [input] The fuel quantity in pounds (lbs) to use for calculation
//! @param aSpeed    [input] The speed to use for calculation
//! @param aAltitude [input] The altitude to use for calculation
//! @return Time in seconds of how long aPounds fuel will last
double WsfFuel::FuelSeconds(const double aPounds, const double aSpeed, const double aAltitude)
{
   double fuelTime = std::numeric_limits<double>::max();
   double rate     = GetConsumptionRate(aSpeed, aAltitude);
   if (rate > 0.0)
   {
      fuelTime = aPounds / rate;
   }
   return fuelTime;
}

// =================================================================================================
//! Gets the time to the given fuel state based on the current consumption rate.
//!
//! @param aFuelState [input] The fuel state to check.
//! @return Time in seconds to the given fuel state.
double WsfFuel::TimeTo(FuelState aFuelState)
{
   WsfPlatform* platformPtr = GetPlatform();

   // Get platform speed
   double speed(platformPtr->GetSpeed());

   // Get platform altitude
   double latNotUsed(0.0);
   double lonNotUsed(0.0);
   double altitude(0.0);
   platformPtr->GetLocationLLA(latNotUsed, lonNotUsed, altitude);

   return TimeTo(aFuelState, altitude, speed);
}

// =================================================================================================
//! Gets the time to the given fuel state based on the consumption rate at the given altitude and speed.
//!
//! @param aFuelState [input] The fuel state to check.
//! @param aAltitude  [input] The desired altitude
//! @param aSpeed     [input] The desired speed
//! @return Time in seconds to the given fuel state.
double WsfFuel::TimeTo(FuelState aFuelState, const double aAltitude, const double aSpeed)
{
   double timeTo = std::numeric_limits<double>::max();
   double rate   = GetNetDepletionRate(aAltitude, aSpeed);
   if (rate > 0.0)
   {
      double quantity = 0.0;
      switch (aFuelState)
      {
      case ON_BINGO:
         quantity = std::max(mCurrentQuantity - mBingoQuantity, 0.0);
         break;
      case ON_RESERVE:
         quantity = std::max(mCurrentQuantity - mReserveQuantity, 0.0);
         break;
      case ON_EMPTY:
         quantity = mCurrentQuantity;
         break;
      default:
         break;
      }
      timeTo = quantity / rate;
   }
   return timeTo;
}

// =================================================================================================
//! Gets the distance at the given fuel state based on the current consumption rate.
//!
//! @param aFuelState [input] The fuel state to check.
//! @return Distance in meters to the given fuel state.
double WsfFuel::DistanceAt(FuelState aFuelState)
{
   WsfPlatform* platformPtr = GetPlatform();

   // Get platform altitude
   double latNotUsed(0.0);
   double lonNotUsed(0.0);
   double altitude(0.0);
   platformPtr->GetLocationLLA(latNotUsed, lonNotUsed, altitude);

   return DistanceAt(aFuelState, altitude, platformPtr->GetSpeed());
}

// =================================================================================================
//! Gets the distance at the given fuel state based on the consumption rate at the given altitude and speed.
//!
//! @param aFuelState [input] The fuel state to check.
//! @param aAltitude  [input] The desired altitude
//! @param aSpeed     [input] The desired speed
//! @return Distance in meters to the given fuel state.
double WsfFuel::DistanceAt(FuelState aFuelState, const double aAltitude, const double aSpeed)
{
   return aSpeed * TimeAt(aFuelState, aAltitude, aSpeed);
}

// =================================================================================================
//! Gets the distance to the given fuel state based on the current consumption rate.
//!
//! @param aFuelState [input] The fuel state to check.
//! @return Distance in meters to the given fuel state.
double WsfFuel::DistanceTo(FuelState aFuelState)
{
   WsfPlatform* platformPtr = GetPlatform();

   // Get platform altitude
   double latNotUsed(0.0);
   double lonNotUsed(0.0);
   double altitude(0.0);
   platformPtr->GetLocationLLA(latNotUsed, lonNotUsed, altitude);

   return DistanceTo(aFuelState, altitude, platformPtr->GetSpeed());
}

// =================================================================================================
//! Gets the distance to the given fuel state based on the consumption rate at the given altitude and speed.
//!
//! @param aFuelState [input] The fuel state to check.
//! @param aAltitude  [input] The desired altitude
//! @param aSpeed     [input] The desired speed
//! @return Distance in meters to the given fuel state.
double WsfFuel::DistanceTo(FuelState aFuelState, const double aAltitude, const double aSpeed)
{
   return aSpeed * TimeTo(aFuelState, aAltitude, aSpeed);
}

// =================================================================================================
//! Completely fill up with fuel (instantaneous).
//! This tells the derived class to set its 'current fuel' to the maximum allowed.
//!
//! @param aSimTime [input] The current simulation time.
//! @return Returns 'true'.
// virtual
bool WsfFuel::Refuel(double aSimTime)
{
   SetQuantityRemaining(mMaximumQuantity);
   ExecuteRefuelScript(aSimTime);
   ResetFlags();
   mLastUpdate = aSimTime;
   return true;
}

double WsfFuel::CalcConsumptionRate()
{
   return GetConsumptionRate();
}

double WsfFuel::CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed)
{
   return GetConsumptionRate();
}

// =================================================================================================
void WsfFuel::ExecuteEmptyScript(double aSimTime)
{
   if (!mEmptyEventTriggered)
   {
      mEmptyEventTriggered = true;

      // Execute registered Callbacks for this event.
      WsfObserver::FuelEvent(GetSimulation())(aSimTime, this, "OUT_OF_FUEL");

      if (mOnEmptyScriptPtr)
      {
         mContextPtr->ExecuteScript(aSimTime, mOnEmptyScriptPtr);
      }
   }
}

// =================================================================================================
void WsfFuel::ExecuteBingoScript(double aSimTime)
{
   if (!mBingoEventTriggered)
   {
      mBingoEventTriggered = true;

      // Execute registered Callbacks for this event.
      WsfObserver::FuelEvent(GetSimulation())(aSimTime, this, "BINGO_FUEL");

      if (mOnBingoScriptPtr)
      {
         mContextPtr->ExecuteScript(aSimTime, mOnBingoScriptPtr);
      }
   }
}

// =================================================================================================
void WsfFuel::ExecuteRefuelScript(double aSimTime)
{
   // Execute registered Callbacks for this event.
   WsfObserver::FuelEvent(GetSimulation())(aSimTime, this, "REFUEL_COMPLETED");

   if (mOnRefuelScriptPtr)
   {
      mContextPtr->ExecuteScript(aSimTime, mOnRefuelScriptPtr);
   }
}

// =================================================================================================
void WsfFuel::ExecuteReserveScript(double aSimTime)
{
   if (!mReserveEventTriggered)
   {
      mReserveEventTriggered = true;

      // Execute registered Callbacks for this event.
      WsfObserver::FuelEvent(GetSimulation())(aSimTime, this, "RESERVE_FUEL");

      if (mOnReserveScriptPtr)
      {
         mContextPtr->ExecuteScript(aSimTime, mOnReserveScriptPtr);
      }
   }
}

// =================================================================================================
//! Completely fill up with fuel (instantaneous).
//! This tells the derived class to set its 'current fuel' to the supplied quantity.
//!
//! @param aSimTime  [input] The current simulation time.
//! @param aQuantity [input] The fuel quantity supplied.
//! @return Always returns 'true'.
// virtual
bool WsfFuel::Refuel(double aSimTime, double aQuantity)
{
   SetQuantityRemaining(aQuantity);
   ExecuteRefuelScript(aSimTime);
   ResetFlags();
   mLastUpdate = aSimTime;
   return true;
}

// =================================================================================================
//! Calculates a delta fuel quantity.
//!
//! @param aSimTime [input] The current simulation time.
//! @return A fuel quantity.
double WsfFuel::CalculateDeltaQuantity(double aSimTime)
{
   // Use Simpson's Rule Integration:
   // (Override this method if fuel can be off-loaded
   // to a receiver, or on-loaded from a tanker.)
   double newConsumeRate = CalcConsumptionRate();

   // If this is the first call then the 'previous consumption rate' must be initialized
   // to the current rate, as there is no previous rate.
   if (mFirstDeltaUpdate)
   {
      mConsumeRate      = newConsumeRate;
      mFirstDeltaUpdate = false;
   }
   double delta = 0.5 * (newConsumeRate + mConsumeRate) * (aSimTime - mLastUpdate);
   mConsumeRate = newConsumeRate;
   return delta;
}

void WsfFuel::IncrementQuantity(double aDelta)
{
   SetQuantityRemaining(mCurrentQuantity += aDelta);
}

void WsfFuel::DecrementQuantity(double aDelta)
{
   SetQuantityRemaining(mCurrentQuantity -= aDelta);
}

// =================================================================================================
//! Executes a fuel object script if a fuel quantity has dropped below a threshold.
//!
//! @param aSimTime [input] The current simulation time.
void WsfFuel::CheckEvents(double aSimTime)
{
   if ((!mEmptyEventTriggered) && (mCurrentQuantity <= 0.0))
   {
      ExecuteEmptyScript(aSimTime);
   }
   else if ((!mReserveEventTriggered) && (mCurrentQuantity <= mReserveQuantity))
   {
      ExecuteReserveScript(aSimTime);
   }
   else if ((!mBingoEventTriggered) && (mCurrentQuantity <= mBingoQuantity))
   {
      ExecuteBingoScript(aSimTime);
   }
}

// =================================================================================================
//! Returns the amount of fuel that will be expended if the platform traveled
//!    the given route at the altitude and speed designated by each waypoint.
//!
//! @param aRoute [input] The Desired Route.
double WsfFuel::GetQuantityRequired(const WsfRoute& aRoute)
{
   double quantityTotal = 0.0;

   int                             prevIdx   = 0;
   int                             currIdx   = 1;
   const WsfRoute::WaypointVector& waypoints = aRoute.GetWaypoints();
   for (; currIdx < (int)waypoints.size(); ++currIdx, ++prevIdx)
   {
      double distance = aRoute.GetDistance(prevIdx, currIdx);
      double altitude = waypoints[prevIdx].GetAlt();
      double speed    = waypoints[prevIdx].GetSpeed();

      quantityTotal += GetQuantityRequired(distance, altitude, speed);
   }
   return quantityTotal;
}

// =================================================================================================
//! Returns the amount of fuel that will be expended if the platform traveled
//!    the given distance at it's current altitude and speed.
//!
//! @param aDistance [input] The desired distance
double WsfFuel::GetQuantityRequired(const double aDistance)
{
   // Get platform altitude
   double latNotUsed(0.0);
   double lonNotUsed(0.0);
   double altitude(0.0);
   GetPlatform()->GetLocationLLA(latNotUsed, lonNotUsed, altitude);

   return GetQuantityRequired(aDistance, altitude, GetPlatform()->GetSpeed());
}

// =================================================================================================
//! Returns the amount of fuel that will be expended if the platform traveled
//!    the given distance at the given altitude and speed.
//!
//! @param aDistance [input] The desired distance
//! @param aAltitude [input] The desired altitude
//! @param aSpeed    [input] The desired speed
double WsfFuel::GetQuantityRequired(const double aDistance, const double aAltitude, const double aSpeed)
{
   // quantity (kg) = (distance (m) / speed (m/s)) * rate (kg/s);
   return (aDistance / aSpeed) * GetConsumptionRate(aAltitude, aSpeed);
}

void WsfFuel::SetPlatform(WsfPlatform* aPlatformPtr)
{
   if (aPlatformPtr != nullptr)
   {
      mContextPtr->SetParent(&aPlatformPtr->GetScriptContext());
   }
   WsfPlatformPart::SetPlatform(aPlatformPtr);
}

const char* WsfFuel::GetScriptClassName() const
{
   return "WsfFuel";
}

const int* WsfFuel::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_FUEL, cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL};
   return roles;
}

void* WsfFuel::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_FUEL)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return (WsfPlatformPart*)this;
   }
   return nullptr;
}
