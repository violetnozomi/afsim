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

#ifndef WSFSAM_LAUNCHCOMPUTER_HPP
#define WSFSAM_LAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <utility>
#include <vector>

#include "TblLookup.hpp"
class UtInput;
class UtInputBlock;
#include "WsfLaunchComputer.hpp"

//! The WsfSAMLaunchComputer will estimate Time-To-Intercept an air threat
//! from a stationary ground (missile or directed energy) weapon system, by
//! using an interpolated table lookup.

class WSF_MIL_EXPORT WsfSAM_LaunchComputer : public WsfLaunchComputer
{
public:
   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_SAM_LAUNCH_COMPUTER"; }

   WsfSAM_LaunchComputer();
   WsfSAM_LaunchComputer(const WsfSAM_LaunchComputer& aSrc);
   WsfSAM_LaunchComputer& operator=(const WsfSAM_LaunchComputer&) = delete;
   ~WsfSAM_LaunchComputer() override                              = default;

   WsfLaunchComputer* Clone() const override;
   bool               ProcessInput(UtInput& aInput) override;

   bool Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;

   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;

   //! This method is for testing of the internal interpolation routines only.
   double InterpolatedTimeToIntercept(double aOffset, double aAltitude, double aRange, double aSpeed);

   class WSF_MIL_EXPORT Interpolator
   {
   public:
      Interpolator()
         : mOffsetLU()
         , mAltLU()
         , mRangeLU()
         , mSpeedLU()
         , mOffsetsPtr(nullptr)
         , mAltitudesPtr(nullptr)
         , mRangesPtr(nullptr)
         , mSpeedsPtr(nullptr)
         , mTOFsPtr(nullptr)
      {
      }
      double              TimeOfFlightFor(double aOffset, double aAltitude, double aRange, double aSpeed);
      TblLookupLU<double> mOffsetLU;
      TblLookupLU<double> mAltLU;
      TblLookupLU<double> mRangeLU;
      TblLookupLU<double> mSpeedLU;

      // These must be set before use:
      const TblIndVarU<double>* mOffsetsPtr;
      const TblIndVarU<double>* mAltitudesPtr;
      const TblIndVarU<double>* mRangesPtr;
      const TblIndVarU<double>* mSpeedsPtr;
      const TblDepVar4<double>* mTOFsPtr;
   };

   class WSF_MIL_EXPORT InterceptEnvelope
   {
   public:
      InterceptEnvelope();
      void         Resize(const std::vector<double>& aOffsets,
                          const std::vector<double>& aAltitudes,
                          const std::vector<double>& aRanges,
                          const std::vector<double>& aSpeeds);
      bool         Initialize(bool aIgnoreTOFs);
      void         ProcessInterceptEnvBlock(UtInputBlock& aInputBlock);
      bool         ProcessInput(UtInput& aInput);
      bool         AssumeLateralSymmetry() const { return mLaterallySymmetric; }
      bool         ResolveDeprecatedInput();
      void         ResizeTOFsAndSetDefaults();
      unsigned int CountValidTOFs();

      void SetTOF_Value(unsigned int aOffsetIndex,
                        unsigned int aAltitudeIndex,
                        unsigned int aRangeIndex,
                        unsigned int aSpeedIndex,
                        double       aTOF_Value);

      size_t NumOffsets() const { return mOffsets.GetSize(); }
      size_t NumAltitudes() const { return mAltitudes.GetSize(); }
      size_t NumRanges() const { return mRanges.GetSize(); }
      size_t NumSpeeds() const { return mSpeeds.GetSize(); }

      double Offset(unsigned int aIndex) const { return mOffsets.Get(aIndex); }
      double Altitude(unsigned int aIndex) const { return mAltitudes.Get(aIndex); }
      double GroundRange(unsigned int aIndex) const { return mRanges.Get(aIndex); }
      double Speed(unsigned int aIndex) const { return mSpeeds.Get(aIndex); }

      // Only needed for deprecated input style.
      struct AltSet
      {
         double                                 alt{0.0};
         std::vector<std::pair<double, double>> mRangeAndTOFs;
      };
      bool ReadRngAndTOF(UtInput& aInput, double& aRange, double& aTOF);

      double TimeOfFlight(unsigned int aOffsetIndex,
                          unsigned int aAltitudeIndex,
                          unsigned int aRangeIndex,
                          unsigned int aSpeedIndex) const
      {
         return mTOFs.Get(aOffsetIndex, aAltitudeIndex, aRangeIndex, aSpeedIndex);
      }

      bool               mLaterallySymmetric;
      TblIndVarU<double> mOffsets;
      TblIndVarU<double> mAltitudes;
      TblIndVarU<double> mRanges;
      TblIndVarU<double> mSpeeds;
      TblDepVar4<double> mTOFs;

      std::vector<AltSet> mObsoleteInput;
   };

   //! Return a pointer to the intercept envelope.
   InterceptEnvelope* GetEnvPtr();

   static bool ValidateEnvelope(InterceptEnvelope* aEnvelopePtr, bool aIgnoreTOFs);

protected:
   struct TestPoint
   {
      TestPoint()
         : latOffset(0)
         , altitude(0)
         , groundRange(0)
         , speed(0.0){};
      double latOffset;
      double altitude;
      double groundRange;
      double speed;
   };
   void TestInterpolationPoints();

   bool ProcessArray(UtInput&             aInput,
                     const std::string&   aBlockTerminator,
                     UtInput::ValueType   aUnitType,
                     std::vector<double>& aValues);

   bool InitializeTTIData() override;

   std::vector<double> mInputOffsets;
   std::vector<double> mInputAltitudes;
   std::vector<double> mInputRanges;
   std::vector<double> mInputSpeeds;

   std::vector<TestPoint>             mTestPoints;
   std::shared_ptr<InterceptEnvelope> mEnvelopePtr;
   Interpolator                       mInterpolator;
};

#endif
