// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSIXDOFFUEL_HPP
#define WSFSIXDOFFUEL_HPP

#include "wsf_six_dof_export.h"

#include "WsfFuel.hpp"
#include "WsfSixDOF_Mover.hpp"

namespace wsf
{
namespace six_dof
{
//! This class derives from WsfFuel and provides a class that provides a simple
//! base class for fuel objects. Functions include fuel consumption rate and
//! current fuel quantity, as well as "Bingo" fuel state and calculations for
//! how far the vehicle can fly with various conditions. WsfSixDOF_Fuel supports
//! as many WsfFuel functions as possible -- in some cases the more advanced
// and higher-fidelity SixDOF fuel model simply cannot be simplified.

class WSF_SIX_DOF_EXPORT Fuel : public WsfFuel
{
public:
   explicit Fuel(WsfScenario& aScenario);
   ~Fuel() = default;

   Fuel& operator=(const Fuel& other) = delete;

   WsfFuel* Clone() const override;

   //! @name WsfPlatformPart methods.
   //@{
   // bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;
   //@}

   // This sets the SixDOF mover for the fuel object
   void SetMover(Mover* aMover) { mMoverPtr = aMover; }

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
   Fuel(const Fuel& aSrc);

   void SetSixDOF_Mover();

   void UpdateSixDOF_FuelData(double aSimTime);

   // This calculates the fuel consumption under current conditions
   double CalcConsumptionRate() override;

   // This calculates the fuel consumption at specified conditions.
   // Note: At the present time, this calculates the fuel consumption under current conditions.
   double CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed) override;

   Mover* mMoverPtr = nullptr;
};
} // namespace six_dof
} // namespace wsf


#endif
