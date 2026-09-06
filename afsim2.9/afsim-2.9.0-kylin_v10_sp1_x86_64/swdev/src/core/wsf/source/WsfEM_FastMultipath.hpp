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

#ifndef WSFEM_FASTMULTIPATH_HPP
#define WSFEM_FASTMULTIPATH_HPP

#include "wsf_export.h"

#include <complex>

#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Propagation.hpp"
#include "WsfEM_Types.hpp"
class WsfEM_XmtrRcvr;

//! Fast multipath propagation model.
class WSF_EXPORT WsfEM_FastMultipath : public WsfEM_Propagation
{
public:
   WsfEM_FastMultipath();
   WsfEM_FastMultipath(const WsfEM_FastMultipath& aSrc);
   WsfEM_FastMultipath& operator=(const WsfEM_FastMultipath& aRhs) = delete;
   ~WsfEM_FastMultipath() override;

   static WsfEM_Propagation* ObjectFactory(const std::string& aTypeName);

   WsfEM_Propagation* Clone() const override;

   double ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment) override;

   virtual double ComputeReflectionGain(WsfEM_XmtrRcvr*                        aXmtrRcvrPtr,
                                        const WsfEM_Interaction::BeamData&     aBeamData,
                                        const WsfEM_Interaction::RelativeData& aRelTgtLoc,
                                        double                                 aDepressionAngle,
                                        double                                 aFrequency,
                                        WsfEM_Types::Polarization              aPolarization);

   bool ProcessInput(UtInput& aInput) override;

   void SetSoilMoistureFraction(double aSoilMoistureFraction);
   void SetSurfaceRoughness(double aSurfaceRoughness);

   static void ComputeReflectionCoefficient(double                      aGrazingAngle,
                                            const std::complex<double>& aDielectricConstant,
                                            WsfEM_Types::Polarization   aPolarization,
                                            double&                     aMagnitude,
                                            double&                     aPhaseShift);

   static bool ComputeReflectionGeometry(double  aEarthRadius,
                                         double  aAntHeight,
                                         double  aTgtSlantRange,
                                         double  aTgtElevation,
                                         double& aAntToRefSlantRange,
                                         double& aRefToTgtSlantRange,
                                         double& aDepressionAngle,
                                         double& aGrazingAngle,
                                         double& aPathLengthDifference);

   static void GetSoilDielectricConstant(double                aFrequency,
                                         double                aMoistureFraction,
                                         std::complex<double>& aDielectricConstant);

private:
   double mSoilMoistureFraction;
   double mSurfaceRoughness;
};

#endif
