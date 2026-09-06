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

#ifndef WSFP6DOF_FUEL_HPP
#define WSFP6DOF_FUEL_HPP

#include "wsf_p6dof_export.h"

#include "WsfFuel.hpp"
#include "WsfP6DOF_Mover.hpp"

//! This class derives from WsfFuel and provides a class that provides a simple
//! base class for fuel objects. Functions include fuel consumption rate and
//! current fuel quantity, as well as "Bingo" fuel state and calculations for
//! how far the vehicle can fly with various conditions. WsfP6DOF_Fuel supports
//! as many WsfFuel functions as possible -- in some cases the more advanced
// and higher-fidelity P6DOF fuel model simply cannot be simplified.

class WSF_P6DOF_EXPORT WsfP6DOF_Fuel : public WsfFuel
{
public:
   explicit WsfP6DOF_Fuel(WsfScenario& aScenario);
   ~WsfP6DOF_Fuel() override = default;

   WsfP6DOF_Fuel& operator=(const WsfP6DOF_Fuel& other) = delete;

   WsfFuel* Clone() const override;

   //! @name WsfPlatformPart methods.
   //@{
   // bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;
   //@}

   // This sets the P6DOF mover for the fuel object
   void SetMover(WsfP6DOF_Mover* aMover) { mMoverPtr = aMover; }

   // Returns the fuel consumption rate in kg/sec
   double GetConsumptionRate() override;

   // Returns the fuel consumption rate in kg/sec
   // For now, this returns the current consumption rate. Later, it will utilize altitude and speed.
   double GetConsumptionRate(const double aAltitude, const double aSpeed) override;

   // Quantity is in kg
   void SetMaximumQuantity(double aQuantity) override;

   // Returns current fuel quantity (kg)
   double GetQuantityRemaining() const override;

   // void SetQuantityRemaining(double aQuantity) override;

   // Instantaneous refueling
   bool Refuel(double aSimTime) override;

   // Quantity is in kg
   bool Refuel(double aSimTime, double aQuantity) override;

protected:
   WsfP6DOF_Fuel(const WsfP6DOF_Fuel& aSrc);

   void SetP6DofMover();

   void UpdateP6DofFuelData(double aSimTime);

   // This calculates the fuel consumption under current conditions
   double CalcConsumptionRate() override;

   // This calculates the fuel consumption at specified conditions.
   // Note: At the present time, this calculates the fuel consumption under current conditions.
   double CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed) override;

   WsfP6DOF_Mover* mMoverPtr;
};

#endif
