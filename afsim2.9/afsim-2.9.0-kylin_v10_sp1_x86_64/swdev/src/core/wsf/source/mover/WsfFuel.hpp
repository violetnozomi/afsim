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

#ifndef WSFFUEL_HPP
#define WSFFUEL_HPP

#include "wsf_export.h"

class UtScript;
#include "WsfPlatformPart.hpp"
class WsfRoute;
class WsfScriptContext;
#include "WsfSinglePlatformObserver.hpp"

//! A simple base class for fuel objects.  The Reserve fuel value defaults to zero unless
//! specified otherwise. Fuel quantity amounts Current, Bingo, Initial, and Maximum
//! include Reserve.
class WSF_EXPORT WsfFuel : public WsfPlatformPart, public WsfSinglePlatformObserver
{
public:
   enum FuelState
   {
      ON_BINGO   = 0,
      ON_RESERVE = 1,
      ON_EMPTY   = 2
   };

   WsfFuel(WsfScenario& aScenario);
   ~WsfFuel() override;

   WsfFuel* Clone() const override;

   //! @name Component infrastructure methods not provided by WsfPlatformPart.
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   int           GetComponentInitializationOrder() const override { return cWSF_INITIALIZE_ORDER_FUEL; }
   //@}

   //! @name Common infrastructure methods.
   //@{
   const char* GetScriptClassName() const override;
   //@}

   //! @name WsfPlatformPart methods.
   //@{
   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;
   // NOTE: Fuel does not utilize TurnOn/TurnOff. It is updated when the mover is updated.
   //       If the mover is off then the fuel is considered to be off, and vice versa.
   //@}

   //! @name Overrides of WsfSinglePlatformObserver
   //@{
   void OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr) override;
   //@}

   //! @name Fuel quantity accessor and mutators
   //@{
   virtual double GetConsumptionRate() { return mConsumeRate; }
   virtual double GetConsumptionRate(const double aAltitude, const double aSpeed);
   void           SetConsumptionRate(double aRate) { mConsumeRate = aRate; }

   virtual double GetSupplyRate() const { return 0.0; }
   virtual double GetReceiveRate() const { return 0.0; }

   virtual double GetNetDepletionRate();
   virtual double GetNetDepletionRate(const double aAltitude, const double aSpeed);

   virtual double GetMaximumQuantity() const { return mMaximumQuantity; }
   virtual void   SetMaximumQuantity(double aQuantity) { mMaximumQuantity = aQuantity; }

   virtual double GetInitialQuantity() const { return mInitialQuantity; }
   virtual void   SetInitialQuantity(double aQuantity) { mInitialQuantity = aQuantity; }

   virtual double GetReserveQuantity() const { return mReserveQuantity; }
   virtual void   SetReserveQuantity(double aQuantity);

   double GetBingoQuantity() const { return mBingoQuantity; }
   void   SetBingoQuantity(double aQuantity);

   virtual double GetQuantityRemaining() const { return mCurrentQuantity; }
   virtual void   SetQuantityRemaining(double aQuantity);

   virtual double GetQuantityRequired(const WsfRoute& aRoute);
   virtual double GetQuantityRequired(const double aDistance);
   virtual double GetQuantityRequired(const double aDistance, const double aAltitude, const double aSpeed);
   //@}

   //! @name Fuel prediction methods
   //@{

   double FuelSecondsRemaining(const WsfRoute& aRoute);
   double FuelSeconds(const double aPounds);
   double FuelSeconds(const double aPounds, const double aSpeed, const double aAltitude);

   //! Returns the time at the given fuel state based on the current consumption rate
   double TimeAt(FuelState aFuelState);
   //! Returns the time at the given fuel state based on the consumption of the given altitude and speed
   double TimeAt(FuelState aFuelState, const double aSpeed, const double aAltitude);

   //! Returns the time to the given fuel state based on the current consumption rate
   double TimeTo(FuelState aFuelState);
   //! Returns the time to the given fuel state based on the consumption of the given altitude and speed
   double TimeTo(FuelState aFuelState, const double aAltitude, const double aSpeed);

   //! Returns the distance at the given fuel state based on the current consumption rate
   double DistanceAt(FuelState aFuelState);
   //! Returns the distance at the given fuel state based on the consumption of the given altitude and speed
   double DistanceAt(FuelState aFuelState, const double aAltitude, const double aSpeed);

   //! Returns the distance to the given fuel state based on the current consumption rate
   double DistanceTo(FuelState aFuelState);
   //! Returns the distance to the given fuel state based on the consumption of the given altitude and speed
   double DistanceTo(FuelState aFuelState, const double aAltitude, const double aSpeed);

   //@}

   bool IsBingo() const { return mBingoEventTriggered; }
   bool IsOnReserve() const { return mReserveEventTriggered; }
   bool IsEmpty() const { return mEmptyEventTriggered; }

   //! Instantaneous refueling.
   virtual bool Refuel(double aSimTime);
   virtual bool Refuel(double aSimTime, double aQuantity);

   virtual WsfStringId GetModeId() const { return mMode; }
   virtual std::string GetMode() const;

   virtual void SetMode(WsfStringId aMode);
   void         SetMode(const std::string& aMode);

   void SetPlatform(WsfPlatform* aPlatformPtr) override;

protected:
   // WsfFuel by default has no update interval
   // Override these methods to get regular Update() calls
   void   SetUpdateInterval(double aInterval) override {}
   double GetUpdateInterval() const override { return 0.0; }

   double GetLastUpdateTime() const { return mLastUpdate; }

   virtual double CalcConsumptionRate();

   // Derived classes should use lookup tables for the consumption rate at different altitudes and speeds
   virtual double CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed);

   // Make internal scripts available to derived classes:
   void ExecuteEmptyScript(double aSimTime);
   void ExecuteReserveScript(double aSimTime);
   void ExecuteBingoScript(double aSimTime);
   void ExecuteRefuelScript(double aSimTime);

   //! Copy constructor (for Clone())
   WsfFuel(const WsfFuel& aSrc);

   //! Assignment constructor
   WsfFuel& operator=(const WsfFuel& aSrc) = delete;

   //! Calculates a delta fuel quantity.
   virtual double CalculateDeltaQuantity(double aSimTime);

   void IncrementQuantity(double aDelta);
   void DecrementQuantity(double aDelta);

   //! Executes a fuel object script if a fuel quantity has dropped below a threshold.
   //! (Transition is only valid when crossing from above to below a threshold value.)
   void CheckEvents(double aSimTime);

   void ResetFlags();

   double mLastUpdate;
   double mReserveQuantity; //! Reserve fuel (defaults to 0)
   double mBingoQuantity;   //! Bingo fuel   (defaults to 0)

private:
   //! Script pointers
   WsfScriptContext* mContextPtr;
   UtScript*         mOnEmptyScriptPtr;
   UtScript*         mOnReserveScriptPtr;
   UtScript*         mOnBingoScriptPtr;
   UtScript*         mOnRefuelScriptPtr;

   double mConsumeRate; // Positive = Decreasing Quantity, Negative = Increasing Quantity

   double mMaximumQuantity; //! Max fuel     (defaults to huge value)
   double mInitialQuantity; //! Initial fuel (defaults to 0)
   double mCurrentQuantity; //! Current fuel (defaults to 0)

   bool mBingoEventTriggered;
   bool mReserveEventTriggered;
   bool mEmptyEventTriggered;
   bool mFirstDeltaUpdate;

   WsfStringId mMode; // Implementation-defined named mode that <may> impact fuel consumption.
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfFuel, cWSF_COMPONENT_FUEL)

#endif
