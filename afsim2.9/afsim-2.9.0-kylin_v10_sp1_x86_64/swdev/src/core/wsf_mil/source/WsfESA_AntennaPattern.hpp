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

#ifndef WSFESAANTENNAPATTERN_HPP
#define WSFESAANTENNAPATTERN_HPP

#include "wsf_mil_export.h"

#include <vector>

class UtInput;
#include "UtRandom.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfObject.hpp"

//! The implementation of an 'Electronically Scanned Array (ESA)'  antenna patterns.
//!
//! Different weighting functions can be applied:
//!    - Uniform (default)
//!    - Taylor
//!

class WSF_MIL_EXPORT WsfESA_AntennaPattern : public WsfAntennaPattern
{
public:
   enum LatticeType
   {
      cLT_RECTANGLULAR,
      cLT_TRIANGLULAR
   };

   enum WeightingFun
   {
      cWT_None    = 0,
      cWT_Taylor  = 1,
      cWT_Uniform = 2
   };

   struct Element
   {
      Element()
         : mWeight(1.0)
      {
         mLocation.Set(0.0);
         mNormal.Set(0.0);
      }

      UtVec3d mLocation; //!< Current Vector of indexed 3D element locations used for pattern calculations
      double  mWeight;   //!< The weighting array for distribution tapering and failed elements.
      UtVec2d mNormal;   //!< Element normal direction (theta, phi) from the array's reference normal.
   };

   using ElementVec = std::vector<Element>;
   using WeightVec  = std::vector<double>;

   class WSF_MIL_EXPORT ESA_Data : public WsfAntennaPattern::BaseData
   {
   public:
      struct TaylorWeightingData
      {
         double mSidelobeLevelX; //!< Azimuth sidelobe level used for weighting functions (ratio).
         double mSidelobeLevelY; //!< Elevation sidelobe level used for weighting functions (ratio).

         //!< Azimuth n-bar parameter for Taylor Sidelobe control (n-bar = num sidelobes controlled + 1)
         int mN_BarX;

         //!< Elevation n-bar parameter for Taylor Sidelobe control (n-bar = num sidelobes controlled + 1)
         int mN_BarY;

         void Clear()
         {
            mSidelobeLevelX = 0.0;
            mSidelobeLevelY = 0.0;
            mN_BarX         = 0;
            mN_BarY         = 0;
         }
      };

      ESA_Data();
      ESA_Data(const ESA_Data& aSrc) = default;
      ~ESA_Data() override           = default;

      bool ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput) override;
      bool Initialize(WsfAntennaPattern& aAntennaPattern) override;

      double mdX;      //!< X-axis element spacing in meters
      double mdY;      //!< Y-axis element spacing in meters
      double mLengthX; //!< Input X-axis length of the array in meters
      double mLengthY; //!< Input Y-axis length of the array in meters
      int    mNX;      //!< Input X-axis number of elements
      int    mNY;      //!< Input Y-axis number of elements

      bool mBackBaffled; //! Back baffled array flag, minimum gain is used if Azimuth angles outside +-90 degs

      double mFailedRatio; //!< Ratio of failed elements in the ESA

      WeightingFun mDistribution; //!< The weighting function to be applied

      TaylorWeightingData mTaylorData; //!< Taylor sidelobe level control data

      //! @note The steering loss is applied in
      //!       WsfEM_Antenna::ComputeBeamSteeringLoss outside this class. This allows it to be extended to other
      //!       antenna pattern classes. An example of how this would be applied is below.
      double mScanExponentX; //!< Azimuth exponent for azimuth distribution function - NOT USED
      double mScanExponentY; //!< Elevation exponent for elevation distribution function - NOT USED

      int mPhaseNumBits; //!< Quantization of the steering phase
      int mAmpNumBits;   //!< Quantization of the amplitude weighting

      LatticeType mLatticeType; //!< The lattice type (i.e. shape) for element location calculations.

      WsfStringId mElementPatternTypeId; //!< The element pattern type Id for loading an element pattern from and
                                         //!< antenna pattern type.

      bool mDebug; //! Debug flag.

   private:
      void ProcessTaylorWeightingInput(UtInput& aInput);
   };

   WsfESA_AntennaPattern(ESA_Data* aEsaData = nullptr);
   ~WsfESA_AntennaPattern() override;

   static WsfAntennaPattern* ObjectFactory(const std::string& aTypeName);

   WsfAntennaPattern* Clone() const override;

   bool Initialize(WsfSimulation* aSimulationPtr) override;

   double GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_EL) override;

   double GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const override;
   double GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const override;
   double GetPeakGain(double aFrequency) const override;

   //! @name Private data access.
   //! These methods return data that should not be considered part of the public interface.
   //! They were provided to support External Services.
   //! Use at your own risk!
   double GetElementSpacingX() const { return mESA_DataPtr->mdX; }
   double GetElementSpacingY() const { return mESA_DataPtr->mdY; }
   int    GetNumberElementsX() const { return mESA_DataPtr->mNX; }
   int    GetNumberElementsY() const { return mESA_DataPtr->mNY; }
   bool   IsDistributionTaylor() const { return mESA_DataPtr->mDistribution == cWT_Taylor; }
   bool   IsDistributionUniform() const { return mESA_DataPtr->mDistribution == cWT_Uniform; }
   double GetTaylorDataSidelobeLevelX() const { return mESA_DataPtr->mTaylorData.mSidelobeLevelX; }
   double GetTaylorDataSidelobeLevelY() const { return mESA_DataPtr->mTaylorData.mSidelobeLevelY; }
   double GetTaylorDataN_BarX() const { return mESA_DataPtr->mTaylorData.mN_BarX; }
   double GetTaylorDataN_BarY() const { return mESA_DataPtr->mTaylorData.mN_BarY; }
   int    GetPhaseNumBits() const { return mESA_DataPtr->mPhaseNumBits; }
   int    GetAmpNumBits() const { return mESA_DataPtr->mAmpNumBits; }
   //@}

protected:
   WsfESA_AntennaPattern(const WsfESA_AntennaPattern& aSrc);

   //! Assignment is not supported.
   WsfESA_AntennaPattern& operator=(const WsfESA_AntennaPattern& aRhs) = delete;

   ESA_Data* GetData() const { return mESA_DataPtr; }

   virtual double ComputeDirectivityGain(double aFrequency, double aEBS_Az, double aEBS_El) const;

   virtual double ComputeElementGain(double aFrequency, double aAzAngle, double aElAngle) const;

   virtual double ComputeElementFactor(double aFrequency, double aEBS_Az, double aEBS_El, double aAzAngle, double aElAngle) const;

   void SetESA_DataPtr(ESA_Data* aESA_DataPtr) { mESA_DataPtr = aESA_DataPtr; }

   virtual void ResizeArray();
   virtual void AdjustApertureElements(ut::Random& aRandom);
   virtual void AdjustElementLocations();

   void ComputeFailedModulesWeights(ut::Random& aRandom);

   //! @name Current ESA data
   ElementVec mElements; //!< Current vector of elements used for pattern calculations
   double     mLengthX;  //!< Current X-axis length of the array in meters
   double     mLengthY;  //!< Current Y-axis length of the array in meters
   int        mNX;       //!< Current X-axis number of elements
   int        mNY;       //!< Current Y-axis number of elements
   //! @}

   WeightVec mWeightVecX;
   WeightVec mWeightVecY;

   //!< Calculated aperture efficiency.
   double mApertureEff;

   double mApertureEffX;
   double mApertureEffY;

   WsfAntennaPattern* mElementPatternPtr;

private:
   double ComputeArrayFactor(double aFrequency, double aEBS_Az, double aEBS_El, double aAzAngle, double aElAngle) const;

   void           ComputeDistributionWeights();
   virtual double ComputeApertureEfficiency();
   double         ComputeQuantizationError(int aNumBits, double aRangePerBit, double aValue) const;

   double ComputeGain(double aFrequency, double aAzAngle, double aElAngle, double aEBS_Az, double aEBS_El) const;

   //! Pointer to the data which is shared in the base WsfAntenna class via a shared pointer.
   ESA_Data* mESA_DataPtr;
};

#endif
