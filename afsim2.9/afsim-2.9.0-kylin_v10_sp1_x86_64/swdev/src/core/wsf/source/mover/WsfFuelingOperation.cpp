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

#include "WsfFuelingOperation.hpp"

#include "WsfFuelObserver.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTankedFuel.hpp"

namespace
{
bool sAllowSimultWingAndCenterlineFueling = true;
bool sAllowSimultBoomAndHoseFueling       = true;
} // namespace

// static
bool WsfFuelingOperation::AllowSimultWingAndCenterlineFueling()
{
   return sAllowSimultWingAndCenterlineFueling;
}

// static
void WsfFuelingOperation::SetAllowSimultWingAndCenterlineFueling(bool aValue)
{
   sAllowSimultWingAndCenterlineFueling = aValue;
}

// static
bool WsfFuelingOperation::AllowSimultBoomAndHoseFueling()
{
   return sAllowSimultBoomAndHoseFueling;
}

// static
void WsfFuelingOperation::SetAllowSimultBoomAndHoseFueling(bool aValue)
{
   sAllowSimultBoomAndHoseFueling = aValue;
}

WsfFuelingOperation::WsfFuelingOperation()
   : mSupplierPtr(nullptr)
   , mReceiverPtr(nullptr)
   , mStartFuelTime(0.0)
   , mLastUpdate(0.0)
   , mXferRate(0.0)
   , mCumFuelXfer(0.0)
   , mCmdDisconnect(false)
   , mInternalDisc(false)
   , mMethod(cNO_METHOD)
   , mStation(cNO_STATION)
{
}

WsfFuelingOperation::WsfFuelingOperation(double         aSimTime,
                                         WsfTankedFuel* aSupplierPtr,
                                         WsfTankedFuel* aReceiverPtr,
                                         MethodType     aMethod,
                                         StationType    aStation,
                                         double         aInitRate)
   : mSupplierPtr(aSupplierPtr)
   , mReceiverPtr(aReceiverPtr)
   , mStartFuelTime(aSimTime)
   , mLastUpdate(aSimTime)
   , mXferRate(aInitRate)
   , mCumFuelXfer(0.0)
   , mCmdDisconnect(false)
   , mInternalDisc(false)
   , mMethod(aMethod)
   , mStation(aStation)
{
   mReceiverPtr->ConnectTanker(aSimTime, mSupplierPtr->GetPlatform()->GetIndex(), mSupplierPtr, mXferRate);
   WsfObserver::TankingEvent(mReceiverPtr->GetSimulation())(aSimTime, this, "REFUEL_INITIATED");
}

// virtual
WsfFuelingOperation::~WsfFuelingOperation()
{
   if (!mInternalDisc)
   {
      DisconnectP(mLastUpdate);
   }
}

//! Supplies a string describing the fuel supply method (hose, boom, NONE).
std::string WsfFuelingOperation::GetMethodName() const
{
   if (mMethod == cBOOM)
   {
      return "boom";
   }
   if (mMethod == cHOSE)
   {
      return "hose";
   }
   else
   {
      return "NONE";
   }
}

//! Supplies a string describing the fuel supply station (left, center, right, NONE).
std::string WsfFuelingOperation::GetStationName() const
{
   if (mStation == cLEFT_WING)
   {
      return "left";
   }
   if (mStation == cCENTERLINE)
   {
      return "center";
   }
   if (mStation == cRIGHT_WING)
   {
      return "right";
   }
   else
   {
      return "NONE";
   }
}

//! This virtual method propagates a refueling transfer forward in time.
//! The input/output parameter aDisconnect serves two purposes, and should be initially set false,
//! unless a forced disconnect is desired, in which it should be set true.  When refueling is
//! terminated, the aDisconnect parameter will be internally set true.
//! @param  aSimTime Current simulated time.
//! @param  aDisconnected Set true if disconnected.
//! @return Returns the Mass quantity of fuel offloaded during the Update.
// virtual
double WsfFuelingOperation::Update(double aSimTime, bool& aDisconnected)
{
   // There are four possible "disconnect" paths (a, b, c, d):
   if (mInternalDisc)
   {
      // a.  This class was already previously disconnected,
      //     but has not been destructed yet, so bail out early.
      aDisconnected = true;
      return 0.0;
   }

   if (mCmdDisconnect)
   {
      aDisconnected = true;
   }

   // b.  Disconnect() was called prior to this update, but mInternalDisc not set true yet.
   // c.  aDisconnected was set true prior to this call.
   // d.  During this Update() a receiver tank is topped off or filled up, or a supplier tank goes Reserve.
   double acceptedFuel = 0.0;
   double deltaT       = aSimTime - mLastUpdate;

   if (deltaT > 0.0)
   {
      // Transfer fuel according to the acceptable client receiving rate:
      double newXferRate = CalcRunXferRate(aSimTime);

      // Simpson's Rule Integration:
      double desiredFuel = 0.5 * (mXferRate + newXferRate) * deltaT;

      // Update the (soon to be) previous value.
      mXferRate = newXferRate;

      double suppliedFuel = mSupplierPtr->DecrementQuantity(aSimTime, desiredFuel);

      // The receiver may potentially reject part of what we try to put in,
      // so compare the initial desired fuel to the accepted fuel.
      // If equal, all is well, but if not equal, then somebody is full
      // and/or empty, so terminate tanking operations:
      acceptedFuel = mReceiverPtr->IncrementQuantity(aSimTime, suppliedFuel);

      double excess = suppliedFuel - acceptedFuel;

      if (excess > 0.0)
      {
         // The receiver did not accept all of the delivered fuel,
         // so put some back into the supplier..
         mSupplierPtr->IncrementQuantity(aSimTime, excess);
      }

      mCumFuelXfer += acceptedFuel;

      mLastUpdate = aSimTime;

      // 0.00001 increment is to prevent roundoff error from intervening
      if (aDisconnected || ((acceptedFuel + 0.00001) < desiredFuel))
      {
         DisconnectP(aSimTime);
         aDisconnected = true;
      }
   }

   return acceptedFuel;
}

// private
void WsfFuelingOperation::DisconnectP(double aSimTime)
{
   mCmdDisconnect = true;
   // Assure that the TankingEvent observations are made ONLY once.
   if (mInternalDisc)
   {
      return;
   }
   mInternalDisc = true;
   WsfObserver::TankingEvent(mReceiverPtr->GetSimulation())(aSimTime, this, "REFUEL_COMPLETED");
}

// ================================== SUBCLASSED METHODS ================

std::string WsfFuelingOperation::SupplyPoint::GetMethodName() const
{
   if (mMethod == cBOOM)
   {
      return "boom";
   }
   if (mMethod == cHOSE)
   {
      return "hose";
   }
   else
   {
      return "NONE";
   }
}

std::string WsfFuelingOperation::SupplyPoint::GetStationName() const
{
   if (mStation == cLEFT_WING)
   {
      return "left";
   }
   if (mStation == cCENTERLINE)
   {
      return "center";
   }
   if (mStation == cRIGHT_WING)
   {
      return "right";
   }
   else
   {
      return "NONE";
   }
}
