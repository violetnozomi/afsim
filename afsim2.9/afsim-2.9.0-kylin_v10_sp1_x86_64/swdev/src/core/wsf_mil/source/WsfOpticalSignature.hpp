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

#ifndef WSFOPTICALSIGNATURE_HPP
#define WSFOPTICALSIGNATURE_HPP

#include "wsf_mil_export.h"

#include <vector>

class UtScriptTypes;
#include "WsfEM_Types.hpp"
class WsfPlatform;
class WsfScenario;
#include "WsfSignature.hpp"

//! An object that represents the optical signature of a platform.
class WSF_MIL_EXPORT WsfOpticalSignature : public WsfSignature
{
public:
   enum
   {
      cSIGNATURE_INDEX = 1 //!< Index of signature within WsfSignatureList (must be unique)
   };

   WsfOpticalSignature();
   WsfOpticalSignature* Clone() const override = 0;

   //! Get the optical signature for a given set of conditions.
   //!
   //! @param aSimTime    [input] The current simulation time.
   //! @param aStateId    [input] The string ID representing the signature state to be used.
   //! @param aAzimuth    [input] The azimuth of the observer with respect to the target.
   //! @param aElevation  [input] The elevation of the observer with respect to the target.
   //! @returns The optical cross section (m^2)
   virtual float GetSignature(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation) = 0;

   //! @name New interface methods to get the projected area and radiant intensity.
   //! These methods may be implemented by derived implementations to provide a more complete
   //! implementation of the 'signature' within the optical spectrum.
   //@{
   virtual bool
   GetProjectedArea(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation, double& aProjectedArea);

   using BandList = std::vector<WsfEM_Types::OpticalBand>;

   virtual bool GetRadiantIntensity(double          aSimTime,
                                    WsfStringId     aState,
                                    const BandList& aBands,
                                    double          aAzimuth,
                                    double          aElevation,
                                    double&         aRadiantIntensity);

   bool UsesInherentContrast() const { return mUsesInherentContrast; }
   //@}

   //! @name Methods to support the actual interface on the platform.
   //! These methods provide the interface from the sensor model to the signature.
   //!
   //! The user makes calls like WsfOpticalSignature::GetSignature(...).
   //@{
   static WsfStringId GetInputType(WsfPlatform* aPlatformPtr);

   static WsfStringId GetState(WsfPlatform* aPlatformPtr);

   static bool SetState(WsfPlatform* aPlatformPtr, WsfStringId aState);

   static float GetScaleFactor(WsfPlatform* aPlatformPtr);

   static bool SetScaleFactor(WsfPlatform* aPlatformPtr, float aScaleFactor);

   static float GetValue(double aSimTime, WsfPlatform* aPlatformPtr, double aAzimuth, double aElevation);

   static bool GetProjectedArea(double       aSimTime,
                                WsfPlatform* aPlatformPtr,
                                double       aAzimuth,
                                double       aElevation,
                                double&      aProjectedArea);

   static bool GetRadiantIntensity(double          aSimTime,
                                   WsfPlatform*    aPlatformPtr,
                                   const BandList& aBands,
                                   double          aAzimuth,
                                   double          aElevation,
                                   double&         aRadiantIntensity);

   static bool UsesInherentContrast(WsfPlatform* aPlatformPtr);
   //@}

   static void RegisterScriptMethods(UtScriptTypes& aScriptTypes);
   static void RegisterInterface(WsfScenario& aScenario);

protected:
   void SetInherentContrast(bool aContrast) { mUsesInherentContrast = aContrast; }

private:
   bool mUsesInherentContrast = true;
};

#endif
