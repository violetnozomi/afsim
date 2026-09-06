// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFESA_NX_ANTENNAPATTERN_HPP
#define WSFESA_NX_ANTENNAPATTERN_HPP

#include <map>

class UtInput;

#include "WsfAntennaPattern.hpp"
#include "WsfESA_AntennaPattern.hpp"
#include "WsfObject.hpp"

//! The implementation of an subdividing 'Electronically Scanned Array (ESA)' antenna patterns.
//!
//! Different weighting functions can be applied:
//!    - Uniform (default)
//!    - Taylor
//!
//! @note Cosine roll-off is applied separate of this when the Antenna mode is set for electronic beam steering, see
//! WsfEM_Antenna
//!
//! Adaption from Skolnik 2nd edition adding in array sub-division and edge angles to model modern threat radar systems
//! making it non-exportable.

class WsfESA_NX_AntennaPattern : public WsfESA_AntennaPattern
{
public:
   struct ArraySubDivideRatios
   {
      double X_Ratio;
      double Y_Ratio;
   };

   using ArraySplitRatios = std::map<int, ArraySubDivideRatios>;

   class ESA_NX_Data : public WsfESA_AntennaPattern::ESA_Data
   {
   public:
      ESA_NX_Data();
      ESA_NX_Data(const ESA_NX_Data& aSrc) = default;
      ~ESA_NX_Data() override              = default;

      bool ProcessInput(WsfAntennaPattern& aAntennaPattern, UtInput& aInput) override;
      bool Initialize(WsfAntennaPattern& aAntennaPattern) override;

      void GetArraySplitRatios(unsigned int aBeamCount, double& aX_SplitRatio, double& aY_SplitRatio);

      //!< The azimuth lean angle of the area, effectively create a trapezoid (radians)
      double mEdgeAngleX;

      //!< The elevation lean angle of the area, effectively create a trapezoid (radians)
      double mEdgeAngleY;

      ArraySplitRatios mArraySplitRatios; //!< Map of beam count to beam splitting ratios
   };

   WsfESA_NX_AntennaPattern();
   ~WsfESA_NX_AntennaPattern() override = default;

   static WsfAntennaPattern* ObjectFactory(const std::string& aTypeName);

   WsfAntennaPattern* Clone() const override;

   bool Initialize(WsfSimulation* aSimulationPtr) override;

   void SetAntennaBeamCount(unsigned int aBeamCount) override;

   //! @name Private data access.
   //! These methods return data that should not be considered part of the public interface.
   //! They were provided to support External Services.
   //! Use at your own risk!
   double GetEdgeAngleX() const { return NX_Data()->mEdgeAngleX; }
   double GetEdgeAngleY() const { return NX_Data()->mEdgeAngleY; }
   double GetElementSpacingX() const { return NX_Data()->mdX; }
   double GetElementSpacingY() const { return NX_Data()->mdY; }
   int    GetNumberElementsX() const { return NX_Data()->mNX; }
   int    GetNumberElementsY() const { return NX_Data()->mNY; }
   bool   IsDistributionTaylor() const { return NX_Data()->mDistribution == cWT_Taylor; }
   bool   IsDistributionUniform() const { return NX_Data()->mDistribution == cWT_Uniform; }
   double GetTaylorDataSidelobeLevelX() const { return NX_Data()->mTaylorData.mSidelobeLevelX; }
   double GetTaylorDataSidelobeLevelY() const { return NX_Data()->mTaylorData.mSidelobeLevelY; }
   double GetTaylorDataN_BarX() const { return NX_Data()->mTaylorData.mN_BarX; }
   double GetTaylorDataN_BarY() const { return NX_Data()->mTaylorData.mN_BarY; }
   int    GetPhaseNumBits() const { return NX_Data()->mPhaseNumBits; }
   int    GetAmpNumBits() const { return NX_Data()->mAmpNumBits; }
   //@}

protected:
   ESA_NX_Data* NX_Data() const { return dynamic_cast<ESA_NX_Data*>(GetData()); }

   WsfESA_NX_AntennaPattern(const WsfESA_NX_AntennaPattern& aSrc);

   //! Assignment is not supported.
   WsfESA_NX_AntennaPattern& operator=(const WsfESA_NX_AntennaPattern& aRhs) = delete;

   double ComputeDirectivityGain(double aFrequency, double aEBS_Az, double aEBS_El) const override;

   void ResizeArray() override;
   void AdjustElementLocations() override;

private:
   WsfSimulation* mSimulationPtr;

   //! @name Current ESA data
   unsigned int mAntennaBeamCount; //!< Current total system beam count for this pattern
                                   //! @}
};

#endif
