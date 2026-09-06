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

#ifndef WSFGENAP_ANTENNAPATTERN_HPP
#define WSFGENAP_ANTENNAPATTERN_HPP

#include "WsfALARM_AntennaPattern.hpp"

//! A class that implements an interface to the GENeralized Antenna Pattern
//! (GENAP) generator from SUPPRESSOR 6.0.
//!
//! The GENeralized Antenna Pattern generator in SUPPRESSOR 6.0 is an interface
//! to a subset of the capabilities that exist in Technical Radar Analysis and
//! Modeling System (TRAMS).
//!
//! @note This class populates data for the base class WsfALARM_AntennaPattern
//! which performs the actual interpolation of the data.
//!
//! @note The interface is only available if WSF_WITH_GENAP=1 defined. The
//!       real genap.f is not supplied with the standard WSF distribution
//!       because it has not been determined if it is government limited
//!       distribution.

class WsfGENAP_AntennaPattern : public WsfALARM_AntennaPattern
{
public:
   //! Illumination distribution function.
   enum Distribution
   {
      cDF_UNKNOWN  = 0,
      cDF_UNIFORM  = 1,
      cDF_COSINE   = 3,
      cDF_COSECANT = 11,
      cDF_BW_SLL   = 13
   };

   //! aperture blockage shape
   enum BlockageShape
   {
      cBS_NONE        = 0,
      cBS_ELLIPTICAL  = 1,
      cBS_RECTANGULAR = 2,
      cBS_DIAMOND     = 3
   };
   //! Data that is shared amongst all instances of a given ALARM antenna pattern
   class GENAP_Data : public WsfALARM_AntennaPattern::ALARM_Data
   {
   public:
      enum
      {
         cNAZMAX = 3601, // As defined in genap.f
         cNELMAX = 1801  // As defined in genap.f
      };

      GENAP_Data();
      ~GENAP_Data() override = default;
      ;

      bool ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput) override;
      bool Initialize(WsfAntennaPattern& aAntennaPattern) override;

      Distribution mAzDistribution;
      Distribution mElDistribution;

      BlockageShape mBlockageShape;

      double mAzExponent;
      double mAzSidelobeLevelInput;
      double mAzSidelobeLevel;
      int    mAzSamples;
      double mAzWeights[cNAZMAX];
      double mAzBlockageWeights[cNAZMAX];
      double mAzBlockage;
      double mAzSidelobePosition;
      double mAzNullPosition;

      double mElRotation;
      double mElExponent;
      double mElSidelobeLevelInput;
      double mElSidelobeLevel;
      double mElCosecantLimit;
      int    mElSamples;
      double mElWeights[cNELMAX];
      double mElBlockageWeights[cNELMAX];
      double mElBlockage;
      double mElEfficiency;
      double mElSidelobePosition;
      double mElNullPosition;

      double mApertureWidth;
      double mApertureHeight;

   private:
      void GeneratePattern();

      void ComputeApertureWeights(double aWavelength);

      void ComputeCrudePattern(double aWavelength);

      void PatternComplexFourierTransform(const double* aWeightingFunction,
                                          int           aNumApertureSamples,
                                          double        aApertureLength,
                                          int           aNumOutputSamples,
                                          double        aFirstAngle,
                                          double        aAngleSpacing,
                                          double*       aGainPattern);

      void PatternFourierTransform(const double* aWeightingFunction,
                                   int           aNumSamples,
                                   double        aApertureLength,
                                   int           aNumOutputSamples,
                                   double        aFirstAngle,
                                   double        aAngleSpacing,
                                   bool          aVolt,
                                   double*       aGainPattern);

      void CalculateBeamwidth(Distribution  aDistributionFunction,
                              int           aMaxNumPoints,
                              double        aMinAngle,
                              double        aAngularSpacing,
                              const double* aAntennaPattern,
                              double&       aBeamwidth,
                              double&       aMaxSidelobe,
                              double&       aSidelobePosition,
                              double&       aNullPosition);

      void ComputeApertureSize(double aBeamwidth, double aSidelobeLevelDB, double aWavelength, double& aApertureSize);

      void ApertureDistribution(Distribution aDistributionFunction,
                                double       aExponent,
                                double       aBroadside,
                                double       aApertureSize,
                                double&      aEfficiency,
                                double       aSidelobeLevelDB,
                                int          aNumSamples,
                                double*      aWeightingFunction);

      void ModifyApertureDistribution(double*       aXAxisDistribution,
                                      int           aNumXPoints,
                                      const double* aYAxisDistribution,
                                      int           aNumYPoints,
                                      double        aBlockageX,
                                      double        aBlockageY,
                                      ApertureShape aApertureShape,
                                      BlockageShape aBlockageShape,
                                      double*       aModifiedXDistribution);

      double IlluminationEfficiency(double*       aXAxisDistribution,
                                    int           aNumXPoints,
                                    const double* aYAxisDistribution,
                                    int           aNumYPoints,
                                    double        aBlockageX,
                                    double        aBlockageY,
                                    ApertureShape aApertureShape,
                                    BlockageShape aBlockageShape);

      void CosecantBeam(double  aExponent,
                        double  aBroadside,
                        double  aApertureSize,
                        int     aNumSamples,
                        double* aRealPattern,
                        double* aImagPattern,
                        double& aEfficiency);

      void CosecantRolloff(int     aVectorSize,
                           double  aMinimumAngle,
                           double  aAngularSpacing,
                           double* aPattern,
                           double  aBeamwidth,
                           double  aRolloff);

      void CosecantFix(int aVectorSize, double aAngularSpacing, double& aMinimumAngle, double* aPattern);

      double InterpolatedValue(const double* aVector, int aNumPoints, double aX);

      void VectorReplace(int aVectorSize, const double* aInputVector, double* aOutputVector);

      void VectorProduct(int aVectorSize, const double* aFirstVector, const double* aSecondVector, double* aOutputVector);

      double ZSum(int aVectorSize, const double* aVector);

      void TaylorOne(double aSidelobeRatio, int aVectorSize, double* aDistribution);

      double BetaFunction(double aBeta);

      double ZeroBessel(double aX);

      double KFactor(Distribution aDistribution, double aExponent);

      double Energy(int aNum, const double* aRealPattern, const double* aImagPattern);
   };

   WsfGENAP_AntennaPattern();

   static WsfAntennaPattern* ObjectFactory(const std::string& aTypeName);

   WsfAntennaPattern* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   //! @name Private data access.
   //! These methods return data that should not be considered part of the public interface.
   //! They were provided to support External Services.
   //! Use at your own risk!
   ApertureShape GetApertureShape() const override { return GenapData()->mApertureShape; }
   Distribution  GetAzDistribution() const { return GenapData()->mAzDistribution; }
   Distribution  GetElDistribution() const { return GenapData()->mElDistribution; }
   double        GetAzExponent() const { return GenapData()->mAzExponent; }
   double        GetElExponent() const { return GenapData()->mElExponent; }
   double        GetElCosecantLimit() const { return GenapData()->mElCosecantLimit; }
   double        GetAzSidelobeLevelInput() const { return GenapData()->mAzSidelobeLevelInput; }
   double        GetElSidelobeLevelInput() const { return GenapData()->mElSidelobeLevelInput; }
   //@}

protected:
   WsfGENAP_AntennaPattern(const WsfGENAP_AntennaPattern& aSrc) = default;

   //! Assignment is not supported.
   WsfGENAP_AntennaPattern operator=(const WsfGENAP_AntennaPattern& aRhs) const;

private:
   GENAP_Data* GenapData() const { return dynamic_cast<GENAP_Data*>(mSharedDataPtr); }
};

#endif
