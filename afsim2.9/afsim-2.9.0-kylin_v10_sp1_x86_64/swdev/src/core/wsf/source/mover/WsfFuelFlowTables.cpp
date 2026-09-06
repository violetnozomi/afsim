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

#include "WsfFuelFlowTables.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfPlatform.hpp"

// ================================ class WsfFuelFlow =================================================

WsfFuelFlow::WsfFuelFlow(UtAtmosphere& aAtmosphere, double aRate)
   : mSpeedInMach(false)
   , mFlowRate(aRate)
   , mPlatPtr(nullptr)
   , mAtmosphere(aAtmosphere)
{
}

//! Copy constructor (for Clone()).
// protected
WsfFuelFlow::WsfFuelFlow(const WsfFuelFlow& aSrc)
   : mSpeedInMach(aSrc.mSpeedInMach)
   , mFlowRate(aSrc.mFlowRate)
   , mPlatPtr(nullptr)
   , mAtmosphere(aSrc.mAtmosphere)
{
}

//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
// virtual
WsfFuelFlow* WsfFuelFlow::Clone() const
{
   return new WsfFuelFlow(*this);
}

//! Initialize the object.
//!
//! @param aPlatformPtr [input] The platform for which the fuel table is defined.
//! @return 'true' if the object was successfully initialized or 'false'
//! if initialization failed.
// virtual
bool WsfFuelFlow::Initialize(WsfPlatform* aPlatformPtr)
{
   mPlatPtr = aPlatformPtr;
   if (mPlatPtr == nullptr)
   {
      ut::log::info() << "WsfFuelFlow must be provided a non-null platform.";
   }
   return mPlatPtr != nullptr;
}

double WsfFuelFlow::CalcConsumptionRate()
{
   return CalcConsumptionRateAltitudeSpeed(GetAlt(), GetSpeed());
}

double WsfFuelFlow::CalcConsumptionRateAltitudeSpeed(const double aAltitude, const double aSpeed)
{
   if (mSpeedInMach)
   {
      double sonicVel = mAtmosphere.SonicVelocity(aAltitude);
      double mach     = aSpeed / sonicVel;
      return CalcConsumptionRateAltitudeSpeedP(aAltitude, mach);
   }
   else
   {
      return CalcConsumptionRateAltitudeSpeedP(aAltitude, aSpeed);
   }
}

double WsfFuelFlow::GetSpeed() const
{
   return mPlatPtr->GetSpeed();
}

double WsfFuelFlow::GetMass() const
{
   return mPlatPtr->GetMass();
}

double WsfFuelFlow::GetAlt() const
{
   double dummy;
   double alt;
   mPlatPtr->GetLocationLLA(dummy, dummy, alt);
   return alt;
}

// ============================== class WsfFuelFlow1D ===================================================

WsfFuelFlow1D::WsfFuelFlow1D(WsfTabularRateFuel::VarType         aType,
                             std::shared_ptr<TblIndVarU<double>> aIVsPtr,
                             std::shared_ptr<TblDepVar1<double>> aRatesPtr,
                             UtAtmosphere&                       aAtmosphere)
   : WsfFuelFlow(aAtmosphere, 0.0)
   , mType(aType)
   , mIVsPtr(aIVsPtr)
   , mRatesPtr(aRatesPtr)
   , mLookup()
{
}

//! Copy constructor (for Clone()).
// protected
WsfFuelFlow1D::WsfFuelFlow1D(const WsfFuelFlow1D& aSrc)
   : WsfFuelFlow(aSrc)
   , mType(aSrc.mType)
   , mIVsPtr(aSrc.mIVsPtr)
   , mRatesPtr(aSrc.mRatesPtr)
   , mLookup(aSrc.mLookup)
{
}

//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
// virtual
WsfFuelFlow* WsfFuelFlow1D::Clone() const
{
   return new WsfFuelFlow1D(*this);
}

// virtual
double WsfFuelFlow1D::CalcConsumptionRateAltitudeSpeedP(const double aAltitude, const double aSpeed)
{
   double IV = 0.0;
   if (mType == WsfTabularRateFuel::cSPEED)
   {
      IV = aSpeed;
   }
   else if (mType == WsfTabularRateFuel::cALTITUDE)
   {
      IV = aAltitude;
   }
   else
   {
      IV = GetMass();
   }

   mLookup.Lookup(*mIVsPtr, IV);
   return TblEvaluate(*mRatesPtr, mLookup);
}

// =============================== class WsfFuelFlow2D ==================================================

WsfFuelFlow2D::WsfFuelFlow2D(WsfTabularRateFuel::VarType         aIV1Type,
                             WsfTabularRateFuel::VarType         aIV2Type,
                             std::shared_ptr<TblIndVarU<double>> aIVs1Ptr,
                             std::shared_ptr<TblIndVarU<double>> aIVs2Ptr,
                             std::shared_ptr<TblDepVar2<double>> aRatesPtr,
                             UtAtmosphere&                       aAtmosphere)
   : WsfFuelFlow(aAtmosphere, 0.0)
   , mIV1Type(aIV1Type)
   , mIV2Type(aIV2Type)
   , mIVs1Ptr(aIVs1Ptr)
   , mIVs2Ptr(aIVs2Ptr)
   , mRatesPtr(aRatesPtr)
   , mLookup1()
   , mLookup2()
{
}

//! Copy constructor (for Clone()).
// protected
WsfFuelFlow2D::WsfFuelFlow2D(const WsfFuelFlow2D& aSrc)
   : WsfFuelFlow(aSrc)
   , mIV1Type(aSrc.mIV1Type)
   , mIV2Type(aSrc.mIV2Type)
   , mIVs1Ptr(aSrc.mIVs1Ptr)
   , mIVs2Ptr(aSrc.mIVs2Ptr)
   , mRatesPtr(aSrc.mRatesPtr)
   , mLookup1(aSrc.mLookup1)
   , mLookup2(aSrc.mLookup2)
{
}

//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
// virtual
WsfFuelFlow* WsfFuelFlow2D::Clone() const
{
   return new WsfFuelFlow2D(*this);
}

// virtual
double WsfFuelFlow2D::CalcConsumptionRateAltitudeSpeedP(const double aAltitude, const double aSpeed)
{
   double IV1 = 0.0;
   if (mIV1Type == WsfTabularRateFuel::cSPEED)
   {
      IV1 = aSpeed;
   }
   else if (mIV1Type == WsfTabularRateFuel::cALTITUDE)
   {
      IV1 = aAltitude;
   }
   else
   {
      IV1 = GetMass();
   }

   double IV2 = 0.0;
   if (mIV2Type == WsfTabularRateFuel::cSPEED)
   {
      IV2 = aSpeed;
   }
   else if (mIV2Type == WsfTabularRateFuel::cALTITUDE)
   {
      IV2 = aAltitude;
   }
   else
   {
      IV2 = GetMass();
   }

   mLookup1.Lookup(*mIVs1Ptr, IV1);
   mLookup2.Lookup(*mIVs2Ptr, IV2);

   return TblEvaluate(*mRatesPtr, mLookup1, mLookup2);
}

// ============================== class WsfFuelFlow3D ===================================================

WsfFuelFlow3D::WsfFuelFlow3D(WsfTabularRateFuel::VarType         aIV1Type,
                             WsfTabularRateFuel::VarType         aIV2Type,
                             WsfTabularRateFuel::VarType         aIV3Type,
                             std::shared_ptr<TblIndVarU<double>> aIVs1Ptr,
                             std::shared_ptr<TblIndVarU<double>> aIVs2Ptr,
                             std::shared_ptr<TblIndVarU<double>> aIVs3Ptr,
                             std::shared_ptr<TblDepVar3<double>> aRatesPtr,
                             UtAtmosphere&                       aAtmosphere)
   : WsfFuelFlow(aAtmosphere, 0.0)
   , mIV1Type(aIV1Type)
   , mIV2Type(aIV2Type)
   , mIV3Type(aIV3Type)
   , mIVs1Ptr(aIVs1Ptr)
   , mIVs2Ptr(aIVs2Ptr)
   , mIVs3Ptr(aIVs3Ptr)
   , mRatesPtr(aRatesPtr)
   , mLookup1()
   , mLookup2()
   , mLookup3()
{
}

//! Copy constructor (for Clone()).
// protected
WsfFuelFlow3D::WsfFuelFlow3D(const WsfFuelFlow3D& aSrc)
   : WsfFuelFlow(aSrc)
   , mIV1Type(aSrc.mIV1Type)
   , mIV2Type(aSrc.mIV2Type)
   , mIV3Type(aSrc.mIV3Type)
   , mIVs1Ptr(aSrc.mIVs1Ptr)
   , mIVs2Ptr(aSrc.mIVs2Ptr)
   , mIVs3Ptr(aSrc.mIVs3Ptr)
   , mRatesPtr(aSrc.mRatesPtr)
   , mLookup1(aSrc.mLookup1)
   , mLookup2(aSrc.mLookup2)
   , mLookup3(aSrc.mLookup3)
{
}

//! Clone this object by creating an identical copy and returning a pointer to it.
//! @returns A pointer to the cloned object.
// virtual
WsfFuelFlow* WsfFuelFlow3D::Clone() const
{
   return new WsfFuelFlow3D(*this);
}

// virtual
double WsfFuelFlow3D::CalcConsumptionRateAltitudeSpeedP(const double aAltitude, const double aSpeed)
{
   double IV1 = 0.0;
   if (mIV1Type == WsfTabularRateFuel::cSPEED)
   {
      IV1 = aSpeed;
   }
   else if (mIV1Type == WsfTabularRateFuel::cALTITUDE)
   {
      IV1 = aAltitude;
   }
   else
   {
      IV1 = GetMass();
   }

   double IV2 = 0.0;
   if (mIV2Type == WsfTabularRateFuel::cSPEED)
   {
      IV2 = aSpeed;
   }
   else if (mIV2Type == WsfTabularRateFuel::cALTITUDE)
   {
      IV2 = aAltitude;
   }
   else
   {
      IV2 = GetMass();
   }

   double IV3 = 0.0;
   if (mIV3Type == WsfTabularRateFuel::cSPEED)
   {
      IV3 = aSpeed;
   }
   else if (mIV3Type == WsfTabularRateFuel::cALTITUDE)
   {
      IV3 = aAltitude;
   }
   else
   {
      IV3 = GetMass();
   }

   mLookup1.Lookup(*mIVs1Ptr, IV1);
   mLookup2.Lookup(*mIVs2Ptr, IV2);
   mLookup3.Lookup(*mIVs3Ptr, IV3);

   return TblEvaluate(*mRatesPtr, mLookup1, mLookup2, mLookup3);
}
