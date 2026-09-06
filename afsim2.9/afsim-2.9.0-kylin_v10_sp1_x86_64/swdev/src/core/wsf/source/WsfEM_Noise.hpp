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

#ifndef WSFEM_NOISE_HPP
#define WSFEM_NOISE_HPP

#include "wsf_export.h"

class UtInput;
class WsfEM_Interaction;

class WSF_EXPORT WsfEM_Noise
{
public:
   enum Season
   {
      cWINTER = 0,
      cSPRING = 1,
      cSUMMER = 2,
      cAUTUMN = 3
   };

   enum Environment
   {
      cBUSINESS    = 0,
      cRESIDENTIAL = 1,
      cRURAL       = 2,
      cQUIET_RURAL = 3
   };

   WsfEM_Noise();
   virtual ~WsfEM_Noise();

   virtual bool   Initialize();
   virtual bool   ProcessInput(UtInput& aInput);
   virtual double ComputeNoise(WsfEM_Interaction& aResult);

protected:
   virtual double ComputeReferenceAtmosphericNoises(int aTimeblock, double aLatitude, double aLongitude);
   virtual void   GetReferenceAtmosphericNoises(WsfEM_Interaction& aResult,
                                                int&               aCurrentTimeblockIndex,
                                                double&            aNoiseCurrent,
                                                int&               aAdjacentTimeblockIndex,
                                                double&            aNoiseAdjacent,
                                                double&            aTimeSlope);
   virtual void   GenerateFrequencyDependence(double  aLatitude,
                                              int     aTimeblockIndex,
                                              double  aFreqMHz,
                                              double  aReferenceNoise,
                                              double& aAdjustedNoise,
                                              double& aDu,
                                              double& aDl,
                                              double& aSigm,
                                              double& aSigu,
                                              double& aSigl);
   virtual double ComputeGalacticNoise(double aFrequency);
   virtual double ComputeManMadeNoise(double aFrequency);

   //! the season for computing atmospheric noise
   static Season mSeason;

   //! the man-made environment
   Environment mEnvironment;
};

#endif
