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

#include "WsfAero2D.hpp"

#include <algorithm>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"

namespace
{
double GetRequiredCL(double aRequiredForce_N, double aReferenceArea_m2, double aDynamicPressure_Npm2)
{
   return aRequiredForce_N / (aDynamicPressure_Npm2 * aReferenceArea_m2);
}

} // namespace

// ================================================================================================
//! Constructor
WsfAero2D::WsfAero2D(const UtAtmosphere& aAtmosphere)
   : WsfAero(aAtmosphere)
   , mAR(2.0)
   , mE(0.95)
   , // Default value of Oswald's Efficiency Factor
   mClMax(1.0)
   , mK(0.0)
   , mCdZeroSubsonic(0.0)
   , mCdZeroSupersonic(0.0)
   , mMachBeginCdRise(0.0)
   , mMachEndCdRise(0.0)
   , mSubsonicCdSlope(0.0)
   , mSupersonicCdSlope(0.0)
   , mMachMaxSupersonic(5.0)
   , mCd0s()
   , mCl(0.0)
   , mCd0(0.00001)
   , mCdTotal(0.0)
   , mType(cINCOMPRESSIBLE)
   , mIndex(0)
   , mLateralForce(0.0)
{
   mK = 1.0 / (UtMath::cPI * mAR * mE);
}

// ================================================================================================
//! Copy constructor, used by Clone().
WsfAero2D::WsfAero2D(const WsfAero2D& aSrc)
   : WsfAero(aSrc)
   , mAR(aSrc.mAR)
   , mE(aSrc.mE)
   , mClMax(aSrc.mClMax)
   , mK(aSrc.mK)
   , mCdZeroSubsonic(aSrc.mCdZeroSubsonic)
   , mCdZeroSupersonic(aSrc.mCdZeroSupersonic)
   , mMachBeginCdRise(aSrc.mMachBeginCdRise)
   , mMachEndCdRise(aSrc.mMachEndCdRise)
   , mSubsonicCdSlope(aSrc.mSubsonicCdSlope)
   , mSupersonicCdSlope(aSrc.mSupersonicCdSlope)
   , mMachMaxSupersonic(aSrc.mMachMaxSupersonic)
   , mCd0s(aSrc.mCd0s)
   , mCl(aSrc.mCl)
   , mCd0(aSrc.mCd0)
   , mCdTotal(aSrc.mCdTotal)
   , mType(aSrc.mType)
   , mIndex(aSrc.mIndex)
   , mLateralForce(aSrc.mLateralForce)
{
}

// ================================================================================================
WsfAero2D::~WsfAero2D() {}

// ================================================================================================
WsfAero* WsfAero2D::Clone() const
{
   return new WsfAero2D(*this);
}

// ================================================================================================
//! Initialize the aerodynamics object.
//! Method will be used to place drag values into increasing order
//! by Mach number, and facilitate run-time interpolations.
//!
//! @return 'True' if object successfully initialized, or 'false' if an error occurred.
// virtual
bool WsfAero2D::Initialize(double aSimTime, UtEntity* aPlatformPtr)
{
   // Will set mInitialized to base class perception:
   WsfAero::Initialize(aSimTime, aPlatformPtr);

   if (mInitialized && !SetInternalValues(aSimTime))
   {
      // Override to false if there is a problem.
      mInitialized = false;
   }

   return mInitialized;
}

// ================================================================================================
//! SetInternalValues is a protected method which initializes the aerodynamics object.
//! It assures that necessary values are set in the object before it is used.
//! Method will make sure that drag table mach numbers (if used) are continuously
//! increasing, to facilitate run-time interpolations.  Depending upon how drag values
//! are specified, it may create Cd versus Mach number drag values to produce the desired
//! transonic drag rise shape.
//!
//! @return 'True' if object successfully initialized, or 'false' if an error occurred.
bool WsfAero2D::SetInternalValues(double aSimTime)
{
   bool result = true;

   // A non-zero value for mClMax assumes that a computation
   // of drag due-to-lift is expected, so some other valid
   // inputs must be provided to calculate it properly...
   if (mClMax != 0.0)
   {
      if (mAR == 0.0)
      {
         auto out = ut::log::error() << "'aero' properties must specify a non-zero 'aspect_ratio'.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatformName();

         result = false;
      }

      if (mE == 0.0)
      {
         auto out = ut::log::error() << "'aero' must specify a non-zero value for 'oswalds_factor'.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatformName();

         result = false;
      }

      if (result)
      {
         mK = 1.0 / (UtMath::cPI * mAR * mE);
      }
   }

   if (mType == cINCOMPRESSIBLE)
   {
      if ((mCd0 == 0.0) && (mCd0s.size() == 0))
      {
         auto out = ut::log::error() << "'aero' 'zero_lift_cd' was not provided.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatformName();

         result = false;
      }
   }
   else if (mType == cCURVE_POINTS)
   {
      if (mCd0s.size() < 2)
      {
         auto out = ut::log::error() << "'aero' must specify at least two Mach numbers for drag curve interpolation.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatformName();
         result = false;
      }
   }
   else if (mType == cPARAMETRIC)
   {
      // Create Cd vs. Mach curve points for the input parameters, in
      // in order from zero to higher Mach numbers.  If the curve is
      // not empty, it has already been created the Object is probably
      // being re-initialized, so do nothing.

      if (mCd0s.size() == 0)
      {
         if (mCdZeroSubsonic > mCdZeroSupersonic)
         {
            auto out = ut::log::error() << "'aero' drag coefficient must rise through the transonic region.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatformName();
            result = false;
         }

         if (mMachBeginCdRise > mMachEndCdRise)
         {
            auto out =
               ut::log::error() << "'aero' beginning transonic Mach number must be greater than ending Mach number.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatformName();
            result = false;
         }

         if (mMachMaxSupersonic <= mMachEndCdRise)
         {
            auto out = ut::log::error() << "'aero' ending supersonic Mach number must be above the transonic range.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatformName();
            result = false;
         }

         // 1.  Zero Mach Cd is a function of the begin drag-rise Mach and Cd,
         //     plus the specified subsonic slope:
         double mach = 0.0;
         double cd   = mCdZeroSubsonic - mSubsonicCdSlope * mMachBeginCdRise;
         mCd0s.push_back(DragPoint(mach, cd));

         // 2.  Next point is the beginning of transonic drag rise:
         mCd0s.push_back(DragPoint(mMachBeginCdRise, mCdZeroSubsonic));

         // 3.  Now fill in a group of points, using a predetermined
         //     representative drag-rise curve
         const int    nPts                    = 14;
         const double dragRise[/* nPts */][2] = {{0.000, 0.000},
                                                 {0.222, 0.018},
                                                 {0.278, 0.033},
                                                 {0.333, 0.048},
                                                 {0.389, 0.077},
                                                 {0.444, 0.147},
                                                 {0.500, 0.257},
                                                 {0.556, 0.477},
                                                 {0.611, 0.706},
                                                 {0.667, 0.844},
                                                 {0.722, 0.908},
                                                 {0.778, 0.954},
                                                 {0.889, 0.991},
                                                 {1.000, 1.000}};

         double machTransRange = mMachEndCdRise - mMachBeginCdRise;
         double cdTransRange   = mCdZeroSupersonic - mCdZeroSubsonic;

         for (int i = 1; i < nPts - 1; ++i)
         {
            mach = mMachBeginCdRise + dragRise[i][0] * machTransRange;
            cd   = mCdZeroSubsonic + dragRise[i][1] * cdTransRange;

            mCd0s.push_back(DragPoint(mach, cd));
         }

         // 4.  End point of Transonic drag rise:
         mCd0s.push_back(DragPoint(mMachEndCdRise, mCdZeroSupersonic));

         // 5.  Last point is the supersonic slope, extrapolated upward:
         cd = mCdZeroSupersonic + mSupersonicCdSlope * (mMachMaxSupersonic - mMachEndCdRise);
         mCd0s.push_back(DragPoint(mMachMaxSupersonic, cd));
      }
   } // end if mType

   if (mDebug)
   {
      double deltaCl = mClMax / 4.0;

      // For this section, using "=" for values
      auto out = ut::log::debug() << "Aero2D debug tests:";
      for (unsigned int j = 0; j < mCd0s.size(); ++j)
      {
         double cdo  = mCd0s[j].mCd0;
         double mach = mCd0s[j].mMach;

         auto note = out.AddNote() << "For mach = " << mach << ", and Cd0 = " << cdo << ":";

         for (double cl = 0; cl < mClMax; cl += deltaCl)
         {
            double cd  = cdo + mK * cl * cl;
            double lod = cl / cd;
            note.AddNote() << "For Cl = " << cl << ", and Cd = " << cd << ": L/D = " << lod;
         }
      }
   }

   return result;
}

// ================================================================================================
// virtual
bool WsfAero2D::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "cd_zero_subsonic")
   {
      aInput.ReadValue(mCdZeroSubsonic);
      aInput.ValueGreater(mCdZeroSubsonic, 0.0);
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cPARAMETRIC;
      }
      else if (mType != cPARAMETRIC)
      {
         throw UtInput::BadValue(aInput, "This 'cd_zero_subsonic' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "cd_zero_supersonic")
   {
      aInput.ReadValue(mCdZeroSupersonic);
      aInput.ValueGreater(mCdZeroSupersonic, 0.0);
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cPARAMETRIC;
      }
      else if (mType != cPARAMETRIC)
      {
         throw UtInput::BadValue(aInput, "This 'cd_zero_supersonic' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "mach_begin_cd_rise")
   {
      aInput.ReadValue(mMachBeginCdRise);
      aInput.ValueGreater(mMachBeginCdRise, 0.0);
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cPARAMETRIC;
      }
      else if (mType != cPARAMETRIC)
      {
         throw UtInput::BadValue(aInput, "This 'mach_begin_cd_rise' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "mach_end_cd_rise")
   {
      aInput.ReadValue(mMachEndCdRise);
      aInput.ValueGreater(mMachEndCdRise, 0.0);
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cPARAMETRIC;
      }
      else if (mType != cPARAMETRIC)
      {
         throw UtInput::BadValue(aInput, "This 'mach_end_cd_rise' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "subsonic_cd_slope")
   {
      aInput.ReadValue(mSubsonicCdSlope);
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cPARAMETRIC;
      }
      else if (mType != cPARAMETRIC)
      {
         throw UtInput::BadValue(aInput, "This 'subsonic_cd_slope' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "supersonic_cd_slope")
   {
      aInput.ReadValue(mSupersonicCdSlope);
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cPARAMETRIC;
      }
      else if (mType != cPARAMETRIC)
      {
         throw UtInput::BadValue(aInput, "This 'supersonic_cd_slope' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "mach_max_supersonic")
   {
      aInput.ReadValue(mMachMaxSupersonic);
      aInput.ValueGreater(mMachMaxSupersonic, 0.0);
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cPARAMETRIC;
      }
      else if (mType != cPARAMETRIC)
      {
         throw UtInput::BadValue(aInput, "This 'mach_max_supersonic' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "cl_max")
   {
      aInput.ReadValue(mClMax);
      aInput.ValueGreaterOrEqual(mClMax, 0.0);
   }
   else if (command == "zero_lift_cd")
   {
      // Note:  For backward compatibility, when drag
      // predictions for compressible flow are not needed:
      aInput.ReadValue(mCd0);
      aInput.ValueGreater(mCd0, 0.0);
      if (mCd0s.size() == 0)
      {
         mType = cINCOMPRESSIBLE;
      }
      else if (mType != cINCOMPRESSIBLE)
      {
         throw UtInput::BadValue(aInput, "This 'zero_lift_cd' input is not valid for drag = f(Mach).");
      }
   }
   else if (command == "mach_and_cd")
   {
      double mach;
      aInput.ReadValue(mach);
      aInput.ValueGreaterOrEqual(mach, 0.0);
      double cd;
      aInput.ReadValue(cd);
      aInput.ValueGreaterOrEqual(cd, 0.0);
      mCd0s.push_back(DragPoint(mach, cd));
      if (mType == cINCOMPRESSIBLE)
      {
         mType = cCURVE_POINTS;
      }
      else if (mType != cCURVE_POINTS)
      {
         throw UtInput::BadValue(aInput, "This 'mach_and_cd' parameter is incompatible with previous inputs.");
      }
   }
   else if (command == "aspect_ratio")
   {
      aInput.ReadValue(mAR);
      aInput.ValueGreater(mAR, 0.0);
   }
   else if (command == "oswalds_factor")
   {
      aInput.ReadValue(mE);
      aInput.ValueGreater(mE, 0.0);
   }
   else
   {
      myCommand = WsfAero::ProcessInput(aInput);
   }

   return myCommand;
}

double WsfAero2D::CalculateDrag(double aAltitude_m, double aMach, double aWeight_N) const
{
   return GetCdTotal(aWeight_N, mRefArea, aAltitude_m, aMach) *
          GetDynamicPressure(aAltitude_m, GetSpeed(aAltitude_m, aMach)) * mRefArea;
}

// ================================================================================================
//! Update() is the primary method of the class.  Using the internal
//! platform pointer, calculate aerodynamic data in the body frame for
//! this motion state (as appropriate):  The input (desired) body-frame
//! forces may not be physically attainable under the current conditions,
//! so the input values are merely requests, and may not be satisfied.
//!
//! @param aSimTime:          Current simulation time.
//! @param aRequestedYForce:  Desired body-frame lateral force (positive to the right).
//! @param aRequestedZForce:  Desired body-frame vertical force (positive down).
//! @param aTVC_Force         Thrust vector control force
//!
//! @note In the base class implementation, roll angles are zero!
//!
void WsfAero2D::Update(double aSimTime, double aRequestedYForce, double aRequestedZForce, double aTVC_Force)
{
   // Calculate mSpeed, mAlt, mMach, mDynPressure:
   WsfAero::Update(aSimTime, aRequestedYForce, aRequestedZForce, aTVC_Force);

   mYForce       = 0.0;
   mZForce       = 0.0;
   mLateralForce = 0.0;
   mMaximumForce = mClMax * mDynPressure * mRefArea;

   if (mType == cINCOMPRESSIBLE)
   {
      // Here Mach is assumed insignificant, in fact may cause issues, so force it zero.
      mMach = 0.0;
   }

   bool   calcForces = false;
   double kY         = 0.0;
   double kZ         = 0.0;
   double kTVC_Y     = 0.0;
   double kTVC_Z     = 0.0;

   // Now check to see that the inputs are attainable:
   if ((mDynPressure > 0.0) && ((aRequestedYForce != 0.0) || (aRequestedZForce != 0.0)))
   {
      calcForces = true;

      double reqLatForce = sqrt(aRequestedYForce * aRequestedYForce + aRequestedZForce * aRequestedZForce);

      double aNeededAeroY = aRequestedYForce;
      double aNeededAeroZ = aRequestedZForce;
      if (aTVC_Force > reqLatForce)
      {
         aTVC_Force  = reqLatForce;
         kTVC_Y      = aRequestedYForce / aTVC_Force;
         kTVC_Z      = aRequestedZForce / aTVC_Force;
         reqLatForce = 0.0;
      }
      else if (aTVC_Force > 0.0)
      {
         double forceRatio = 1.0 - aTVC_Force / reqLatForce;
         reqLatForce -= aTVC_Force;
         kTVC_Y       = std::max(std::min((aRequestedYForce / aTVC_Force), 1.0), -1.0);
         kTVC_Z       = std::max(std::min((aRequestedZForce / aTVC_Force), 1.0), -1.0);
         aNeededAeroY = aRequestedYForce * forceRatio;
         aNeededAeroZ = aRequestedZForce * forceRatio;
      }

      if (reqLatForce > 0.0)
      {
         kY = aNeededAeroY / reqLatForce;
         kZ = aNeededAeroZ / reqLatForce;
      }

      mCl = GetRequiredCL(reqLatForce, mRefArea, mDynPressure);

      if (mCl > mClMax)
      {
         // Requested forces cannot be attained,
         // reduce them to only what is available:
         mCl = mClMax;

         // If one channel (pitch or yaw) is very "greedy", and we are going to
         // be limited by aerodynamics, let the less greedy channel have all
         // it needs, and limit the greedy one only to what is left over...
         if ((fabs(kY) > 0.9) && (fabs(aNeededAeroZ) < mMaximumForce))
         {
            kZ = aNeededAeroZ / mMaximumForce;
            kY = sqrt(1.0 - kZ * kZ) * UtMath::Sign(kY);
         }
         else if ((fabs(kZ) > 0.9) && (fabs(aNeededAeroY) < mMaximumForce))
         {
            kY = aNeededAeroY / mMaximumForce;
            kZ = sqrt(1.0 - kY * kY) * UtMath::Sign(kZ);
         }
      }
   }
   else
   {
      mCl = 0.0;
   }

   CalcCdTotal();

   mDrag = mCdTotal * mDynPressure * mRefArea;

   if (calcForces)
   {
      mLateralForce = mCl * mDynPressure * mRefArea + aTVC_Force;

      // Now determine how much of this lateral
      // force acts in each direction:
      mYForce = kY * mLateralForce + kTVC_Y * aTVC_Force;
      mZForce = kZ * mLateralForce + kTVC_Z * aTVC_Force;
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "WsfAero2D::Calculate() results: ";
      out.AddNote() << "Requested Y Force: " << aRequestedYForce;
      out.AddNote() << "Requested Z Force: " << aRequestedZForce;
      out.AddNote() << "Requested TVC Force: " << aTVC_Force;
      out.AddNote() << "Mach: " << mMach;
      out.AddNote() << "Cl: " << mCl;
      out.AddNote() << "Cd0: " << mCd0;
      out.AddNote() << "Cdi: " << mCdTotal - mCd0;
      out.AddNote() << "CdT: " << mCdTotal;
      out.AddNote() << "L/D: " << mCl / mCdTotal;
      out.AddNote() << "Drag: " << mDrag;
      out.AddNote() << "Actual Y Force: " << mYForce;
      out.AddNote() << "Actual Z Force: " << mZForce;
   }
}

// ================================================================================================
void WsfAero2D::CalcCd0()
{
   mCd0 = GetCd0(mMach);
}

// ================================================================================================
void WsfAero2D::CalcCdTotal()
{
   if (mType != cINCOMPRESSIBLE)
   {
      CalcCd0();
   }
   mCdTotal = mCd0 + mK * mCl * mCl;
}

// ================================================================================================
double WsfAero2D::GetCd0(double aMach) const
{
   double cd0 = 0;

   // Do not extrapolate, if outside bounds, set Cd = endpoints
   if (aMach <= mCd0s[0].mMach)
   {
      cd0 = mCd0s[0].mCd0;
   }
   else if (aMach >= mCd0s[mCd0s.size() - 1].mMach)
   {
      // Set index to two before the end.
      cd0 = mCd0s[mCd0s.size() - 1].mCd0;
   }
   else
   {
      size_t index = 0;
      // Pick off an x value in the table that is
      // just below the current mach number...
      while (aMach > mCd0s[index + 1].mMach)
      {
         ++index;
      }
      while (aMach < mCd0s[index].mMach)
      {
         --index;
      }

      double dM     = aMach - mCd0s[index].mMach;
      double DM     = mCd0s[index + 1].mMach - mCd0s[index].mMach;
      double interp = dM / DM;
      double DCd    = mCd0s[index + 1].mCd0 - mCd0s[index].mCd0;
      cd0           = mCd0s[index].mCd0 + interp * DCd;
   }
   return cd0;
}

double WsfAero2D::GetCdTotal(double aRequiredForce_N, double aReferenceArea_m2, double aAltitude_m, double aMach) const
{
   double dynamicPressureNpm2 = GetDynamicPressure(aAltitude_m, GetSpeed(aAltitude_m, aMach));
   if (UtMath::NearlyZero(dynamicPressureNpm2))
   {
      return 0.0;
   }
   double CL = GetRequiredCL(aRequiredForce_N, aReferenceArea_m2, dynamicPressureNpm2);

   if (CL > mClMax)
   {
      return 0.0;
   }

   return GetCd0(aMach) + mK * CL * CL;
}
