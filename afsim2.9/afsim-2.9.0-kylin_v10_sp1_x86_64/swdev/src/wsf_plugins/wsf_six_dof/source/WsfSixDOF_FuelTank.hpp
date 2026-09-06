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

#ifndef WSFSIXDOFFUELTANK_HPP
#define WSFSIXDOFFUELTANK_HPP

#include "wsf_six_dof_export.h"

#include <cinttypes>
#include <string>

class UtInput;
#include "UtVec3dX.hpp"
#include "WsfSixDOF_MassProperties.hpp"
#include "WsfSixDOF_Object.hpp"

namespace wsf
{
namespace six_dof
{
class PropulsionSystem;

// The FuelTank class serves as a fuel container for propulsion objects.
// It maintains a quantity of fuel and provides functions to allow other
// objects to take or add fuel from/to the container (fuel tank).
class WSF_SIX_DOF_EXPORT FuelTank : public Object
{
   friend class PropulsionSystem;

public:
   explicit FuelTank();
   ~FuelTank();

   FuelTank& operator=(const FuelTank& other) = delete;

   FuelTank* Clone() const;

   bool ProcessInput(UtInput& aInput);
   bool Initialize(int64_t aSimTime_nanosec);

   // Update should be called every frame. It clears the temp attributes and
   // sets the current attributes. The other 'update functions' (UpdateFuelBurn,
   // UpdateFuelFill, and UpdateFuelTransfer) should be called prior to calling
   // the Update function.
   void Update(int64_t aSimTime_nanosec);

   // This returns a pointer to the mass properties or null, if none exist
   const MassProperties& GetMassProperties() const { return mMassProperties; }

   // SetPropulsionSystem sets the PropulsionSystem parent
   void SetPropulsionSystem(PropulsionSystem* aParent) { mPropulsionSystem = aParent; }

   // FuelFlowPathIntact returns true if there is a viable path for fuel to flow.
   // This essentially confirms that the tank is 'connected' to the fuel system
   // and not in a tank that is separate from the system getting the fuel
   bool FuelFlowPathIntact(const PropulsionSystem* aSystemDrawingFuel);

   // CalculateFuelBurn determines if the requested amount of fuel can be
   // available for burning and also provides the new fuel mass and cg
   // location. It does not change the state of the fuel object. To change
   // the state, call UpdateFuelBurn instead.
   bool CalculateFuelBurn(double    aDeltaT_sec,
                          double    aFuelBurnRequest_lbs,
                          double&   aFuelActuallyProvided_lbs,
                          double&   aNewFuelMass_lbs,
                          UtVec3dX& aCgLocation_ft);

   // UpdateFuelBurn burns the requested amount of fuel and confirms that
   // the fuel is fully available for burning. It also provides the new
   // fuel mass and cg location. It changes the state of the fuel object.
   // To perform calculations without changing state, call CalculateFuelBurn
   // instead.
   bool UpdateFuelBurn(double    aDeltaT_sec,
                       double    aFuelBurnRequest_lbs,
                       double&   aFuelActuallyProvided_lbs,
                       double&   aNewFuelMass_lbs,
                       UtVec3dX& aCgLocation_ft);

   // CalculateFuelFill determines if the requested amount of fuel can be
   // added to the fuel container and also provides the new fuel mass and
   // cg location. It does not change the state of the fuel object. To
   // change the state, call UpdateFuelFill instead.
   // Fuel fill implies external refueling while fuel transfer implies tank-to-tank.
   bool CalculateFuelFill(double    aDeltaT_sec,
                          double    aFuelAddRequest_lbs,
                          double&   aFuelActuallyProvided_lbs,
                          double&   aNewFuelMass_lbs,
                          UtVec3dX& aCgLocation_ft);

   // UpdateFuelFill adds the requested amount of fuel and to confirms that
   // the fuel can be added to the container. It also provides the new fuel
   // mass and cg location. It changes the state of the fuel object. To
   // perform calculations without changing state, call CalculateFuelFill
   // instead.
   // Fuel fill implies external refueling while fuel transfer implies tank-to-tank.
   bool UpdateFuelFill(double    aDeltaT_sec,
                       double    aFuelAddRequest_lbs,
                       double&   aFuelActuallyProvided_lbs,
                       double&   aNewFuelMass_lbs,
                       UtVec3dX& aCgLocation_ft);

   // CalculateFuelTransfer determines if the requested amount of fuel can be
   // added to the fuel container and also provides the new fuel mass and
   // cg location. It does not change the state of the fuel object. To
   // change the state, call UpdateFuelTransfer instead. The aFuelAddRequest_lbs
   // can be either positive (adding fuel) or negative (removing fuel).
   // Fuel fill implies external refueling while fuel transfer implies tank-to-tank.
   bool CalculateFuelTransfer(double    aDeltaT_sec,
                              double    aFuelAddRequest_lbs,
                              double&   aFuelActuallyProvided_lbs,
                              double&   aNewFuelMass_lbs,
                              UtVec3dX& aCgLocation_ft);

   // UpdateFuelTransfer adds the requested amount of fuel and to confirms that
   // the fuel can be added to the container. It also provides the new fuel
   // mass and cg location. It changes the state of the fuel object. To
   // perform calculations without changing state, call CalculateFuelTransfer
   // instead. The aFuelAddRequest_lbs can be either positive (adding fuel) or
   // negative (removing fuel).
   // Fuel fill implies external refueling while fuel transfer implies tank-to-tank.
   bool UpdateFuelTransfer(double    aDeltaT_sec,
                           double    aFuelAddRequest_lbs,
                           double&   aFuelActuallyProvided_lbs,
                           double&   aNewFuelMass_lbs,
                           UtVec3dX& aCgLocation_ft);

   // SetMaxFuelQuantity sets the maximum amount of fuel in the container.
   void SetMaxFuelQuantity(double aFuelQuantity_lbs);

   // GetFuelCapacity_lbs returns the maximum amount of fuel (capacity) of the container.
   double GetFuelCapacity_lbs() const;

   // GetPercentFull returns the current percent full -- 100.0 is returned if full
   double GetPercentFull() const;

   // SetCurrentFuelQuantity sets the current amount of fuel in the container.
   void SetCurrentFuelQuantity(double aFuelQuantity_lbs);

   // GetCurrentFuelQuantity_lbs returns the current amount of fuel in the container.
   double GetCurrentFuelQuantity_lbs() const;

   // GetFuelFraction returns a normalized value of current fuel to maximum fuel
   // for the container. For example, 0.4 is returned when 40% fuel is remaining.
   double GetFuelFraction() const;

   // GetFuelFlow_pps returns the current fuel flow rate in lbs per second.
   double GetFuelFlow_pps() const;

   // GetFuelFlow_pph returns the current fuel flow rate in lbs per hour.
   double GetFuelFlow_pph() const;

   // GetFuelFillRate_pps returns the current fuel fill rate in lbs per second.
   double GetFuelFillRate_pps() const;

   // GetFuelFillRate_pph returns the current fuel fill rate in lbs per hour.
   double GetFuelFillRate_pph() const;

   // GetMaxFuelTransferRate_pps returns the max fuel transfer rate in lbs per second.
   double GetMaxFuelTransferRate_pps() const;

   // GetFuelTransferRate_pps returns the current fuel transfer rate in lbs per second.
   double GetFuelTransferRate_pps() const;

   // GetFuelTransferRate_pph returns the current fuel transfer rate in lbs per hour.
   double GetFuelTransferRate_pph() const;

   // SetFullCgLocation_ft sets the cg location relative to its parent when the
   // container is full. This is typically the mid-point of the fuel tank.
   void SetFullCgLocation_ft(const UtVec3dX& aCgLocation_ft);

   // SetEmptyCgLocation_ft sets the cg location relative to its parent when the
   // container is empty. This is typically the bottom of the fuel tank.
   void SetEmptyCgLocation_ft(const UtVec3dX& aCgLocation_ft);

   // GetCurrentCgLocation_ft returns the current cg location relative to its parent.
   // It is based on the full and empty cg locations and the ratio of current fuel
   // to maximum fuel.
   UtVec3dX GetCurrentCgLocation_ft() const;

   // GetFullCgLocation_ft returns the full cg location relative to its parent.
   UtVec3dX GetFullCgLocation_ft() const;

   // GetEmptyCgLocation_ft returns the empty cg location relative to its parent.
   UtVec3dX GetEmptyCgLocation_ft() const;

   // SetMaxFlowRate_pps sets the maximum flow rate supported by the tank.
   // This function is typically used for testing.
   void SetMaxFlowRate_pps(double aMaxFlowRate_pps) { mMaxFlowRate_pps = aMaxFlowRate_pps; }

   // This calculates the current mass properties, including those
   // from fuel tanks as well as some thrust producers such as
   // solid-propellant rockets
   void CalculateCurrentMassProperties();

protected:
   explicit FuelTank(const FuelTank& aSrc);

   // CalcCgLocation_ft returns cg location relative to its parent, based on fuel quantity
   UtVec3dX CalcCgLocation_ft(double aFuelQuantity_lbs);

   PropulsionSystem* mPropulsionSystem = nullptr;
   MassProperties    mMassProperties;

   double mMaxFlowRate_pps     = 0.0; // Maximum output fuel rate to engines
   double mMaxFillRate_pps     = 0.0; // Maximum fuel input rate during refueling
   double mMaxTransferRate_pps = 0.0; // Maximum fuel transfer rate between tanks
   double mMaxQuantity_lbs     = 0.0; // Maximum fuel quantity of tank

   double mCurrentFuelFlow_pps     = 0.0; // Current fuel flow rate to engines
   double mCurrentFillRate_pps     = 0.0; // Current fuel fill rate
   double mCurrentTransferRate_pps = 0.0; // Current fuel transfer rate
   double mCurrentQuantity_lbs     = 0.0; // Current amount of fuel in tank

   double mTempCurrentFuelFlow_pps     = 0.0; // Current fuel flow rate to engines
   double mTempCurrentFillRate_pps     = 0.0; // Current fuel fill rate
   double mTempCurrentTransferRate_pps = 0.0; // Current fuel transfer rate

   UtVec3dX mCurrentCgLocation_ft; // Current fuel cg location, rel to parent
   UtVec3dX mFullCgLocation_ft;    // Typically the center of the tank
   UtVec3dX mEmptyCgLocation_ft;   // Typically the bottom of the tank
   UtVec3dX mCgEmptyToFullVector;  // Vector pointing from empty to full

   int64_t mLastSimTime_nanosec = 0;
};
} // namespace six_dof
} // namespace wsf

#endif
