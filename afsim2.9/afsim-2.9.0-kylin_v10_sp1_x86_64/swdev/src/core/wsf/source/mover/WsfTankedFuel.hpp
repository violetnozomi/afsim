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

#ifndef WSFTANKEDFUEL_HPP
#define WSFTANKEDFUEL_HPP

#include "wsf_export.h"

#include "WsfFuelingOperation.hpp"
#include "WsfTabularRateFuel.hpp"

//! A class which models a tanked quantity of fuel.
//!
//! This class models a tank of liquid fuel, which may be consumed from and externally refueled.
//! As a resource for a Tanker, it may feed fuel to more than one client (Receiver) simultaneous-
//! ly, but may receive from only one supplier (Tanker) at a time.  Preferred quantity levels may
//! be specified that indicate a desired refueling 'hysteresis band' of fuel to maintain during
//! ferry missions, when constantly in proximity to a Tanker.  Scripted responses may be defined
//! that react when a fuel level rises above or falls below this band.
class WSF_EXPORT WsfTankedFuel : public WsfTabularRateFuel
{
public:
   //! String ID of this type of software object.
   static WsfStringId TypeId();

   // Constructors/Destructor
   WsfTankedFuel(WsfScenario& aScenario);
   WsfTankedFuel& operator=(const WsfTankedFuel&) = delete;
   ~WsfTankedFuel() override;

   WsfFuel* Clone() const override;

   bool Initialize(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   void Update(double aSimTime) override;

   void PlatformDeleted(WsfPlatform* aPlatformPtr) override;

   //! Boolean accessor indicating that this tank is currently supplying fuel to
   //! one (or more) Receiver fuel tanks.
   bool SupplyInProcess() const;

   //! Boolean accessor indicating that this tank is currently receiving fuel
   //! from a supplier tank.
   bool ReceiveInProcess() const { return mReceiveInProcess; }

   //! Maximum mass flow rate for this tank while receiving fuel.
   double MaxReceiveRate() const { return mMaxReceiveRate; }

   //! Method requesting to disconnect the fuel supplier to this tank (if any),
   //! and simultaneously terminate all operations supplying fuel to other tanks.
   bool TerminateAll(double aSimTime);

   //! Method requesting to terminate all supplying operations to other tanks.
   bool DisconnectFuelingOps(double aSimTime);

   //! Method requesting to disconnect the tank supplying fuel into this tank.
   bool DisconnectFromTanker(double aSimTime);

   //! Method requesting to disconnect all fueling operations to/from the
   //! specified platform.
   bool DisconnectFrom(double aSimTime, WsfPlatform* aPlatformPtr);

   //! Returns the current rate at which this tank is supplying fuel to another.
   double CurrentSupplyRate() const { return mCurrentSupplyRate; }

   //! Returns the current rate at which this tank is receiving fuel from another.
   double CurrentReceiveRate() const { return mCurrentReceiveRate; }

   //! Returns the desired operating band maximum fuel.  (Aerial refueling cannot
   //! completely fill the tank, so it is less than maximum.)
   double GetOpBandMaximum() const { return mOperBandMaximum; }

   //! Returns an indication that this tank is enabled to supply fuel to another.
   bool IsSupplyCapable() const { return !mStations.empty(); }

   //! Returns an indication that this tank is enabled to receive fuel from another.
   bool IsReceiveCapable() const { return mReceiveMethod != WsfFuelingOperation::cNO_METHOD; }

   //! Returns an indication that this tank is enabled to receive fuel, and is not currently
   //! receiving fuel from another.
   bool IsReceiveReady() const { return IsReceiveCapable() && !ReceiveInProcess(); }

   //! Returns an enumeration indicating how this tank may receive fuel.
   WsfFuelingOperation::MethodType ReceiveMethod() const { return mReceiveMethod; }

   //! Returns an indication that a fuel supply point on this aircraft is available
   //! to provide fuel to the specified Receiver tank.
   //! @param aReceiverPtr The tank needing fuel.
   //! @param aStationIter If successful, an iterator to the station available for the receiver.
   //! @return True if search was successful.  (If false, aStationIter is not valid.)
   bool FindStationFor(WsfTankedFuel* aReceiverPtr, WsfFuelingOperation::SupplyPointIter& aStationIter);

   //! Called by a fuel "Receiver" (client) processor, while attempting to link
   //! up with a "Tanker" (supplier) for a refueling operation.
   //! @param aSimTime Current simulated time.
   //! @param aReceiverPtr Tank that will receive the offloaded fuel.
   //! @return True indicates success.
   bool BeginSupplyingFuelTo(double aSimTime, WsfTankedFuel* aReceiverPtr);

   //! Returns the sum of all 'supply' fueling operations in progress
   double GetSupplyRate() const override { return mCurrentSupplyRate; }

   //! Returns the rate of a 'receive' fueling operation in progress (if any).
   double GetReceiveRate() const override { return mCurrentReceiveRate; }

   //! This public method is intended to be called only from WsfFuelingOperation:
   //! @param aSimTime Current simulated time.
   //! @param aTankerIndex Platform Index of the Tanker which will fuel the tank.
   //! @param aSourcePtr Pointer to the supplying tank of fuel.
   //! @param aTransferRate The rate of transfer fo fuel from the tanker to the receiver.
   //! @return Returns true if the connection was successful.
   bool ConnectTanker(double aSimTime, size_t aTankerIndex, WsfTankedFuel* aSourcePtr, double aTransferRate);

   //! Increment the quantity of fuel in the tank.
   //! @param aSimTime Current simulated time.
   //! @param aDelta Specifies a desired amount of fuel to push into the tank,
   //! @return Returns the ACTUAL amount of fuel accepted (tank may be full).
   virtual double IncrementQuantity(double aSimTime, double aDelta);

   //! Decrement the quantity of fuel in the tank.
   //! @param aSimTime Current simulated time.
   //! @param aDelta Specifies a desired amount of fuel to be supplied by the tank.
   //! @return Returns the ACTUAL amount of fuel supplied (tank may be empty).
   virtual double DecrementQuantity(double aSimTime, double aDelta);

   //! Set the preferred method to be used when initiating the next fuel supply operation to a Receiver.
   //! @param aSupplyMethod The desired fueling method.
   void SetSupplyMethodPreference(WsfFuelingOperation::MethodType aSupplyMethod) { mSupplyMethPref = aSupplyMethod; }

   //! Set the preferred location to be used in initiating the next fuel supply operation to a Receiver.
   //! @param aLocationPref  The preferred locaion.
   void SetSupplyLocationPreference(WsfFuelingOperation::Preference aLocationPref) { mSupplyLocPref = aLocationPref; }

protected:
   WsfTankedFuel(const WsfTankedFuel& aSrc);

   bool SetDefaultSupplyConfig();

   virtual double UpdateFuelingOps(double aSimTime);

private:
   // A collection of possible tanker fueling supply locations:
   WsfFuelingOperation::SupplyPoints mStations;
   WsfFuelingOperation::MethodType   mReceiveMethod;
   WsfFuelingOperation::MethodType   mSupplyMethPref;
   WsfFuelingOperation::Preference   mSupplyLocPref;

   bool mReceiveInProcess;
   bool mTankInstantly; // ignore any specified transfer rates

   bool mIsBelowTriggered;
   bool mIsAboveTriggered;

   size_t         mSupplierIndex;
   WsfTankedFuel* mSupplyTankPtr;

   double mLastFoUpdate;
   double mMaxReceiveRate;

   double mCurrentReceiveRate;
   double mCurrentSupplyRate;

   double mOperBandMaximum; // Desired maximum quantity obtained if refueled externally (<= Maximum + Reserve)
   double mOperBandMinimum; // Minimum "full" quantity if refueled externally (<= mOperBandMaximum)
};

#endif
