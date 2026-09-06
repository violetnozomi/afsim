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

#ifndef WSFFUELFLOWTABLES_HPP
#define WSFFUELFLOWTABLES_HPP

#include "wsf_export.h"

#include "TblLookup.hpp"
#include "WsfPlatform.hpp"
#include "WsfTabularRateFuel.hpp"

//! Constant flow rate fuel consumption table object.
class WSF_EXPORT WsfFuelFlow
{
public:
   WsfFuelFlow(UtAtmosphere& aAtmosphere, double aRate);
   WsfFuelFlow& operator=(const WsfFuelFlow&) = delete;
   virtual ~WsfFuelFlow()                     = default;
   virtual WsfFuelFlow* Clone() const;

   //! @name Common infrastructure methods.
   //@{
   virtual bool Initialize(WsfPlatform* aPlatformPtr);
   //@}

   //! Calculates the current fuel consumption rate.
   //! @return Returns the current consumption rate.
   virtual double CalcConsumptionRate(); // calling this uses the platform's current altitude and speed
   virtual double CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed);

   void SetTableSpeedIsInMach(bool aSpeedInMach) { mSpeedInMach = aSpeedInMach; }

protected:
   double GetSpeed() const;
   double GetMass() const;
   double GetAlt() const;

   // derived classes should use the lookup table on the given altitude and speed
   virtual double CalcConsumptionRateAltitudeSpeedP(const double aAltitude, const double aSpeed) { return mFlowRate; }

   //! Copy constructor (for Clone()).
   WsfFuelFlow(const WsfFuelFlow& aSrc);

   bool         mSpeedInMach;
   double       mFlowRate;
   WsfPlatform* mPlatPtr; // The platform the fuel flow is computed for.
   UtAtmosphere mAtmosphere;
};

//!   One-Dimensional flow rate fuel consumption table object.
//!   The owning object should allocate memory for the Dependent and Independent
//!   variable arrays, and never delete it, so the table lookup data will be
//!   continuously available during run time.
class WSF_EXPORT WsfFuelFlow1D : public WsfFuelFlow
{
public:
   WsfFuelFlow1D(WsfTabularRateFuel::VarType         aType,
                 std::shared_ptr<TblIndVarU<double>> aIVsPtr,
                 std::shared_ptr<TblDepVar1<double>> aRatesPtr,
                 UtAtmosphere&                       aAtmosphere);
   WsfFuelFlow1D& operator=(const WsfFuelFlow1D&) = delete;
   ~WsfFuelFlow1D() override                      = default;

   WsfFuelFlow* Clone() const override;

protected:
   //! Copy constructor (for Clone()).
   WsfFuelFlow1D(const WsfFuelFlow1D& aSrc);

   double CalcConsumptionRateAltitudeSpeedP(const double aAltitude, const double aSpeed) override;

   WsfTabularRateFuel::VarType         mType;
   std::shared_ptr<TblIndVarU<double>> mIVsPtr;   //<! Independent values
   std::shared_ptr<TblDepVar1<double>> mRatesPtr; //<! Dependent Values (fuel flow rates) = f(IV);
   TblLookupLU<double>                 mLookup;
};

//!   Two-Dimensional flow rate fuel consumption table object.
class WSF_EXPORT WsfFuelFlow2D : public WsfFuelFlow
{
public:
   WsfFuelFlow2D(WsfTabularRateFuel::VarType         aIV1Type,
                 WsfTabularRateFuel::VarType         aIV2Type,
                 std::shared_ptr<TblIndVarU<double>> aIVs1Ptr,
                 std::shared_ptr<TblIndVarU<double>> aIVs2Ptr,
                 std::shared_ptr<TblDepVar2<double>> aRatesPtr,
                 UtAtmosphere&                       aAtmosphere);
   WsfFuelFlow2D& operator=(const WsfFuelFlow2D&) = delete;
   ~WsfFuelFlow2D() override                      = default;
   WsfFuelFlow* Clone() const override;

protected:
   //! Copy constructor (for Clone()).
   WsfFuelFlow2D(const WsfFuelFlow2D& aSrc);

   double CalcConsumptionRateAltitudeSpeedP(const double aAltitude, const double aSpeed) override;

   WsfTabularRateFuel::VarType         mIV1Type;
   WsfTabularRateFuel::VarType         mIV2Type;
   std::shared_ptr<TblIndVarU<double>> mIVs1Ptr;  //<! Independent values
   std::shared_ptr<TblIndVarU<double>> mIVs2Ptr;  //<! Independent values
   std::shared_ptr<TblDepVar2<double>> mRatesPtr; //<! Dependent Values (fuel flow rates) = f(IV);
   TblLookupLU<double>                 mLookup1;
   TblLookupLU<double>                 mLookup2;
};

//!   Three-Dimensional flow rate fuel consumption table object.
class WSF_EXPORT WsfFuelFlow3D : public WsfFuelFlow
{
public:
   WsfFuelFlow3D(WsfTabularRateFuel::VarType         aIV1Type,
                 WsfTabularRateFuel::VarType         aIV2Type,
                 WsfTabularRateFuel::VarType         aIV3Type,
                 std::shared_ptr<TblIndVarU<double>> aIVs1Ptr,
                 std::shared_ptr<TblIndVarU<double>> aIVs2Ptr,
                 std::shared_ptr<TblIndVarU<double>> aIVs3Ptr,
                 std::shared_ptr<TblDepVar3<double>> aRatesPtr,
                 UtAtmosphere&                       aAtmosphere);
   ~WsfFuelFlow3D() override = default;
   WsfFuelFlow3D& operator=(const WsfFuelFlow3D&) = delete;
   WsfFuelFlow*   Clone() const override;

   //! @name External Services access.
   //! These methods support the External Services and are not part of the WSF public interface.
   //@{
   const WsfTabularRateFuel::VarType&        GetIV1Type() const { return mIV1Type; }
   const WsfTabularRateFuel::VarType&        GetIV2Type() const { return mIV2Type; }
   const WsfTabularRateFuel::VarType&        GetIV3Type() const { return mIV3Type; }
   const std::shared_ptr<TblIndVarU<double>> GetIVs1Ptr() const { return mIVs1Ptr; }
   const std::shared_ptr<TblIndVarU<double>> GetIVs2Ptr() const { return mIVs2Ptr; }
   const std::shared_ptr<TblIndVarU<double>> GetIVs3Ptr() const { return mIVs3Ptr; }
   const std::shared_ptr<TblDepVar3<double>> GetRatesPtr() const { return mRatesPtr; }
   //@}

protected:
   //! Copy constructor (for Clone()).
   WsfFuelFlow3D(const WsfFuelFlow3D& aSrc);

   double CalcConsumptionRateAltitudeSpeedP(const double aAltitude, const double aSpeed) override;

   WsfTabularRateFuel::VarType mIV1Type;
   WsfTabularRateFuel::VarType mIV2Type;
   WsfTabularRateFuel::VarType mIV3Type;

   std::shared_ptr<TblIndVarU<double>> mIVs1Ptr; //<! Independent values
   std::shared_ptr<TblIndVarU<double>> mIVs2Ptr; //<! Independent values
   std::shared_ptr<TblIndVarU<double>> mIVs3Ptr; //<! Independent values

   std::shared_ptr<TblDepVar3<double>> mRatesPtr; //<! Dependent Values (fuel flow rates) = f(IV);

   TblLookupLU<double> mLookup1;
   TblLookupLU<double> mLookup2;
   TblLookupLU<double> mLookup3;
};

#endif
